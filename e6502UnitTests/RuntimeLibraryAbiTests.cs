using System;
using System.Collections.Generic;
using System.Globalization;
using System.IO;
using System.Linq;
using System.Text;
using System.Text.RegularExpressions;
using e6502.GameServer.Protocol;
using e6502.GameServer.Server;
using Microsoft.VisualStudio.TestTools.UnitTesting;

namespace e6502UnitTests;

[TestClass]
public class RuntimeLibraryAbiTests
{
    [TestMethod]
    public void PascalRuntimeArchiveContainsOnlyCompleteNobjMembers()
    {
        byte[] library = File.ReadAllBytes(RepoPath(
            "software", "languages", "novapascal", "tools", "build", "PASCAL.NLIB"));

        CollectionAssert.AreEqual(new byte[] { (byte)'N', (byte)'L', (byte)'I', (byte)'B' }, library[..4]);
        Assert.AreEqual(2, library[4], "NLIB version");
        int offset = 6;
        for (int member = 0; member < library[5]; member++)
        {
            Assert.IsTrue(offset + 2 <= library.Length, $"NLIB member {member} is missing its length.");
            int length = library[offset] | library[offset + 1] << 8;
            offset += 2;
            Assert.IsTrue(offset + length <= library.Length, $"NLIB member {member} overruns the archive.");
            AssertCompleteNobj(library.AsSpan(offset, length), member);
            offset += length;
        }

        Assert.AreEqual(library.Length, offset, "NLIB must not contain undeclared members or trailing bytes.");
    }

    [TestMethod]
    public void PascalFileRuntimeRelocatesEveryInternalAbsoluteJump()
    {
        byte[] library = File.ReadAllBytes(RepoPath(
            "software", "languages", "novapascal", "tools", "build", "PASCAL.NLIB"));

        int memberOffset = 6;
        for (int member = 0; member < library[5]; member++)
        {
            int memberLength = library[memberOffset] | library[memberOffset + 1] << 8;
            int objectOffset = memberOffset + 2;
            ReadOnlySpan<byte> objectFile = library.AsSpan(objectOffset, memberLength);
            int symbolCount = objectFile[10] | objectFile[11] << 8;
            int symbolOffset = objectFile[12] | objectFile[13] << 8;
            var symbolOffsets = new int[symbolCount];
            int symbolCursor = symbolOffset;
            bool exportsRead = false;
            for (int symbol = 0; symbol < symbolCount; symbol++)
            {
                symbolOffsets[symbol] = symbolCursor;
                int nameLength = objectFile[symbolCursor + 4];
                exportsRead |= objectFile[symbolCursor + 3] == 1 &&
                               Encoding.ASCII.GetString(objectFile.Slice(symbolCursor + 5, nameLength)) == "READ";
                symbolCursor += 5 + nameLength;
            }

            if (exportsRead)
            {
                int codeOffset = 20 + 8 + objectFile[22];
                int relocationCount = objectFile[14] | objectFile[15] << 8;
                int relocationOffset = objectFile[16] | objectFile[17] << 8;
                Assert.AreEqual(6, relocationCount,
                    "Each absolute control transfer within the relocatable file-runtime member needs an NOBJ relocation.");
                for (int relocation = 0; relocation < relocationCount; relocation++)
                {
                    int record = relocationOffset + relocation * 8;
                    int patch = objectFile[record + 2] | objectFile[record + 3] << 8;
                    int target = objectFile[record + 4] | objectFile[record + 5] << 8;
                    Assert.AreEqual(1, objectFile[record + 1], "Internal control transfers use ABS16 relocations.");
                    Assert.AreEqual(relocation < 5 ? 0x4C : 0x20, objectFile[codeOffset + patch - 1],
                        "The relocation must patch the expected JMP or lazy-Printer JSR operand.");
                    Assert.AreEqual(0, objectFile[symbolOffsets[target] + 2],
                        "Internal jump targets must remain in the member's CODE section.");
                    if (relocation < 5)
                    {
                        Assert.AreEqual(0, objectFile[symbolOffsets[target] + 3],
                            "Internal jump targets must not leak into the archive's global export index.");
                    }
                    else
                    {
                        int targetNameLength = objectFile[symbolOffsets[target] + 4];
                        Assert.AreEqual("REWRITE",
                            Encoding.ASCII.GetString(objectFile.Slice(symbolOffsets[target] + 5, targetNameLength)),
                            "Printer's lazy open must relocate its call to the existing REWRITE entry.");
                    }
                }
                return;
            }

            memberOffset = objectOffset + memberLength;
        }

        Assert.Fail("PASCAL.NLIB has no member exporting READ.");
    }

    [TestMethod]
    public void IndexedAddressHelperIsCanonicalNdkCodeSharedWithPascal()
    {
        string declarations = File.ReadAllText(RepoPath("software", "runtime", "asm", "array.inc"));
        string implementation = File.ReadAllText(RepoPath("software", "runtime", "asm", "array.s"));
        string pascalRuntime = File.ReadAllText(RepoPath("software", "languages", "novapascal", "pascal_runtime.s"));

        StringAssert.Contains(declarations, ".global i_array_index_addr_stack");
        StringAssert.Contains(declarations, ".global i_array_index_byte_stack");
        StringAssert.Contains(implementation, ".include \"array_index_core.body\"");
        StringAssert.Contains(implementation, ".include \"array_index_byte_stack.body\"");
        StringAssert.Contains(pascalRuntime, ".include \"array_index_core.body\"");
        StringAssert.Contains(pascalRuntime, ".include \"array_index_byte_stack.body\"");
        StringAssert.Contains(pascalRuntime, "\"I_ARRAY_INDEX_ADDR_STACK\"");
        StringAssert.Contains(pascalRuntime, "\"I_ARRAY_INDEX_BYTE_STACK\"");
        Assert.IsFalse(pascalRuntime.Contains("index_address_stack:", StringComparison.Ordinal),
            "Pascal must package the canonical NDK implementation instead of maintaining a private copy.");
        Assert.IsTrue(pascalRuntime.IndexOf("index_object:", StringComparison.Ordinal) <
                      pascalRuntime.IndexOf("byte_index_object:", StringComparison.Ordinal));
        Assert.IsTrue(pascalRuntime.IndexOf("byte_index_object:", StringComparison.Ordinal) <
                      pascalRuntime.IndexOf("block_object:", StringComparison.Ordinal),
            "Independent array operations must remain separate archive members for linker extraction.");
    }

    [TestMethod]
    public void PascalConsoleCompatibilityUsesCanonicalKeysAndVisibleHighVideoFields()
    {
        string systemAbi = File.ReadAllText(RepoPath("software", "runtime", "asm", "libsystem.inc"));
        string runtime = File.ReadAllText(RepoPath("software", "languages", "novapascal", "pascal_runtime.s"));

        foreach (string key in new[] { "SYS_KEY_LEFT", "SYS_KEY_RIGHT", "SYS_KEY_UP", "SYS_KEY_DOWN" })
            StringAssert.Contains(systemAbi, key,
                "SYS_WAIT_KEY results must have canonical public names shared by every language runtime.");
        StringAssert.Contains(runtime, "CMP   #SYS_KEY_DOWN");
        StringAssert.Contains(runtime, "LDA   #$18", "Turbo Pascal cursor-down compatibility is ^X.");
        StringAssert.Contains(runtime, "STA   VGC_TEXT_BG",
            "HighVideo must capture an opaque background in every subsequently written field cell.");
        StringAssert.Contains(runtime, "LDA   #VGC_TEXT_BG_TRANSPARENT",
            "LowVideo/NormVideo must restore Nova's normal transparent text background.");
    }

    [TestMethod]
    public void MemoryWindowMappingConsumesTheFullAllocatedXramAddress()
    {
        string abi = File.ReadAllText(RepoPath("software", "runtime", "asm", "libmemory.inc"));
        string module = File.ReadAllText(RepoPath("software", "modules", "memory", "memory.s"));

        StringAssert.Contains(abi, "MEM_MAP_WINDOW  = $18  ; (xaddr:ARG0 u24");
        StringAssert.Contains(module,
            "LDA   LIB_ARG0+2\n      STA   XMC_BANK",
            "A mapped window must stay in the allocator-returned 24-bit XRAM bank.");
    }

    [TestMethod]
    public void ExecutableLifecycleIsSharedByTheNdkMemoryModule()
    {
        string coreAbi = File.ReadAllText(RepoPath("software", "runtime", "asm", "libabi.inc"));
        string memoryAbi = File.ReadAllText(RepoPath("software", "runtime", "asm", "libmemory.inc"));
        string memoryModule = File.ReadAllText(RepoPath("software", "modules", "memory", "memory.s"));
        string pascalRuntime = File.ReadAllText(RepoPath("software", "languages", "novapascal", "pascal_runtime.s"));

        StringAssert.Contains(coreAbi, "LIB_EXEC_SP      = LIB_MBOX+31");
        StringAssert.Contains(memoryAbi, "MEM_EXEC_IMAGE = $1B");
        StringAssert.Contains(memoryAbi, "MEM_EXIT_IMAGE = $1C");
        StringAssert.Contains(memoryModule, ".word   mem_exit_image-1");
        StringAssert.Contains(pascalRuntime, "PASCAL_MEMORY_CALL MEM_EXIT_IMAGE",
            "Pascal Halt must use the same executable lifecycle as every other Nova runtime.");
    }

    [TestMethod]
    public void GameServerAssemblyProtocolConstantsMatchServer()
    {
        string inc = File.ReadAllText(RepoPath("software", "runtime", "asm", "gameserver.inc"));
        IReadOnlyDictionary<string, int> constants = ParseCa65NumericConstants(inc);

        Assert.AreEqual(NovaGameProtocol.ServerProtocolMajor, constants["NGS_SERVER_PROTO_MAJOR"]);
        Assert.AreEqual(NovaGameProtocol.ServerProtocolMinor, constants["NGS_SERVER_PROTO_MINOR"]);
        Assert.AreEqual(NovaGameProtocol.DefaultPort, constants["NGS_DEFAULT_PORT"]);
        Assert.AreEqual(NovaGameProtocol.EnvelopeSize, constants["NGS_ENVELOPE_SIZE"]);
        Assert.AreEqual(NovaGameProtocol.MaxFramePayload, constants["NGS_MAX_FRAME_PAYLOAD"]);
        Assert.AreEqual(NovaGameProtocol.MaxMessagePayload, constants["NGS_MAX_MESSAGE_PAYLOAD"]);

        AssertKind(MessageKind.Hello, "NGS_KIND_HELLO");
        AssertKind(MessageKind.Welcome, "NGS_KIND_WELCOME");
        AssertKind(MessageKind.Error, "NGS_KIND_ERROR");
        AssertKind(MessageKind.Ping, "NGS_KIND_PING");
        AssertKind(MessageKind.Pong, "NGS_KIND_PONG");
        AssertKind(MessageKind.LobbyChat, "NGS_KIND_LOBBY_CHAT");
        AssertKind(MessageKind.UserListRequest, "NGS_KIND_USER_LIST_REQUEST");
        AssertKind(MessageKind.UserListItem, "NGS_KIND_USER_LIST_ITEM");
        AssertKind(MessageKind.UserJoined, "NGS_KIND_USER_JOINED");
        AssertKind(MessageKind.UserLeft, "NGS_KIND_USER_LEFT");
        AssertKind(MessageKind.TableChat, "NGS_KIND_TABLE_CHAT");
        AssertKind(MessageKind.GameListRequest, "NGS_KIND_GAME_LIST_REQUEST");
        AssertKind(MessageKind.GameListItem, "NGS_KIND_GAME_LIST_ITEM");
        AssertKind(MessageKind.TableListRequest, "NGS_KIND_TABLE_LIST_REQUEST");
        AssertKind(MessageKind.TableListItem, "NGS_KIND_TABLE_LIST_ITEM");
        AssertKind(MessageKind.TableCreate, "NGS_KIND_TABLE_CREATE");
        AssertKind(MessageKind.TableJoin, "NGS_KIND_TABLE_JOIN");
        AssertKind(MessageKind.TableLeave, "NGS_KIND_TABLE_LEAVE");
        AssertKind(MessageKind.TableObserve, "NGS_KIND_TABLE_OBSERVE");
        AssertKind(MessageKind.TableCreated, "NGS_KIND_TABLE_CREATED");
        AssertKind(MessageKind.TableJoined, "NGS_KIND_TABLE_JOINED");
        AssertKind(MessageKind.TableLeft, "NGS_KIND_TABLE_LEFT");
        AssertKind(MessageKind.TableEvent, "NGS_KIND_TABLE_EVENT");
        AssertKind(MessageKind.PlayerAction, "NGS_KIND_PLAYER_ACTION");
        AssertKind(MessageKind.StateRequest, "NGS_KIND_STATE_REQUEST");
        AssertKind(MessageKind.StateChunk, "NGS_KIND_STATE_CHUNK");
        AssertKind(MessageKind.AiListRequest, "NGS_KIND_AI_LIST_REQUEST");
        AssertKind(MessageKind.AiListItem, "NGS_KIND_AI_LIST_ITEM");
        AssertKind(MessageKind.AiAddToTable, "NGS_KIND_AI_ADD_TO_TABLE");
        AssertKind(MessageKind.OpponentListRequest, "NGS_KIND_OPPONENT_LIST_REQUEST");
        AssertKind(MessageKind.OpponentListItem, "NGS_KIND_OPPONENT_LIST_ITEM");

        AssertFlag(MessageFlags.Response, "NGS_FLAG_RESPONSE");
        AssertFlag(MessageFlags.Error, "NGS_FLAG_ERROR");
        AssertFlag(MessageFlags.More, "NGS_FLAG_MORE");
        AssertFlag(MessageFlags.Event, "NGS_FLAG_EVENT");

        AssertError(ProtocolErrorCode.BadMessage, "NGS_ERRCODE_BAD_MESSAGE");
        AssertError(ProtocolErrorCode.UnsupportedProtocol, "NGS_ERRCODE_UNSUPPORTED_PROTOCOL");
        AssertError(ProtocolErrorCode.HandleInUse, "NGS_ERRCODE_HANDLE_IN_USE");
        AssertError(ProtocolErrorCode.NotRegistered, "NGS_ERRCODE_NOT_REGISTERED");
        AssertError(ProtocolErrorCode.UnknownGame, "NGS_ERRCODE_UNKNOWN_GAME");
        AssertError(ProtocolErrorCode.UnknownTable, "NGS_ERRCODE_UNKNOWN_TABLE");
        AssertError(ProtocolErrorCode.BadAction, "NGS_ERRCODE_BAD_ACTION");
        AssertError(ProtocolErrorCode.TableFull, "NGS_ERRCODE_TABLE_FULL");
        AssertError(ProtocolErrorCode.UnsupportedMessage, "NGS_ERRCODE_UNSUPPORTED_MESSAGE");
        AssertError(ProtocolErrorCode.InvalidHandle, "NGS_ERRCODE_INVALID_HANDLE");
        AssertError(ProtocolErrorCode.NotSeated, "NGS_ERRCODE_NOT_SEATED");
        AssertError(ProtocolErrorCode.NotParticipant, "NGS_ERRCODE_NOT_PARTICIPANT");
        AssertError(ProtocolErrorCode.UnknownAiProvider, "NGS_ERRCODE_UNKNOWN_AI_PROVIDER");
        AssertError(ProtocolErrorCode.Timeout, "NGS_ERRCODE_TIMEOUT");

        Assert.AreEqual((int)OpponentKind.Human, constants["NGS_OPPONENT_HUMAN"]);
        Assert.AreEqual((int)OpponentKind.Ai, constants["NGS_OPPONENT_AI"]);

        void AssertKind(MessageKind kind, string symbol) =>
            Assert.AreEqual((int)kind, constants[symbol], symbol);

        void AssertFlag(MessageFlags flag, string symbol) =>
            Assert.AreEqual((int)flag, constants[symbol], symbol);

        void AssertError(ProtocolErrorCode code, string symbol) =>
            Assert.AreEqual((int)code, constants[symbol], symbol);
    }

    [TestMethod]
    public void GameServerAssemblyClientUsesLinkerStorageAndStableRoutineAbi()
    {
        string inc = File.ReadAllText(RepoPath("software", "runtime", "asm", "gameserver.inc"));
        string impl = File.ReadAllText(RepoPath("software", "runtime", "asm", "gameserver.s"));

        string[] stateSymbols =
        [
            "NGS_SLOT",
            "NGS_PORTL",
            "NGS_PORTH",
            "NGS_SEQ",
            "NGS_BUFL",
            "NGS_BUFH",
            "NGS_LEN",
            "NGS_LAST_LEN",
            "NGS_VERSION",
            "NGS_KIND",
            "NGS_RECV_SEQ",
            "NGS_FLAGS",
            "NGS_RESULT",
            "NGS_READL",
            "NGS_READH",
            "NGS_REMAIN",
            "NGS_STRPTRL",
            "NGS_STRPTRH",
            "NGS_STRLEN",
            "NGS_VALUE_L",
            "NGS_VALUE_H",
            "NGS_CLIENT_CAPL",
            "NGS_CLIENT_CAPH",
            "NGS_GAME_IDL",
            "NGS_GAME_IDH",
            "NGS_GAME_PROTO_MAJOR",
            "NGS_GAME_PROTO_MINOR",
            "NGS_TABLE_IDL",
            "NGS_TABLE_IDH",
            "NGS_ROLE",
            "NGS_ACTION_TYPE",
            "NGS_TIMEOUT_L",
            "NGS_TIMEOUT_H",
            "NGS_LAST_STATUS",
            "NGS_EXPECTED_KIND",
            "NGS_ERROR_CODE",
            "NGS_ERROR_TEXTPTRL",
            "NGS_ERROR_TEXTPTRH",
            "NGS_ERROR_TEXTLEN"
        ];

        foreach (string symbol in stateSymbols)
        {
            AssertNoNvrAlias(inc, symbol);
            StringAssert.Contains(inc, $".global {symbol}");
            StringAssert.Contains(impl, $"{symbol}:");
        }

        string[] routines =
        [
            "ngs_init",
            "ngs_set_buffer",
            "ngs_set_string",
            "ngs_connect",
            "ngs_connect_default",
            "ngs_disconnect",
            "ngs_status",
            "ngs_ready",
            "ngs_send_current",
            "ngs_recv_current",
            "ngs_set_timeout",
            "ngs_wait_connected",
            "ngs_wait_message",
            "ngs_wait_kind",
            "ngs_parse_error",
            "ngs_ping_roundtrip",
            "ngs_begin_message",
            "ngs_write_byte",
            "ngs_write_u16",
            "ngs_write_string",
            "ngs_write_bytes",
            "ngs_parse_envelope",
            "ngs_read_byte",
            "ngs_read_u16",
            "ngs_read_string",
            "ngs_build_hello",
            "ngs_build_ping",
            "ngs_build_lobby_chat",
            "ngs_build_table_chat",
            "ngs_build_user_list_request",
            "ngs_build_game_list_request",
            "ngs_build_table_list_request",
            "ngs_build_table_create",
            "ngs_build_table_join",
            "ngs_build_table_observe",
            "ngs_build_table_leave",
            "ngs_build_player_action",
            "ngs_build_state_request",
            "ngs_build_ai_list_request",
            "ngs_build_ai_add_to_table",
            "ngs_build_opponent_list_request"
        ];

        foreach (string routine in routines)
        {
            StringAssert.Contains(inc, $".global {routine}");
            StringAssert.Contains(impl, $".export {routine}");
        }

        StringAssert.Contains(impl, ".segment \"BSS\"");
        // NIC dependency is declared through the header for linker resolution from
        // nova.lib (the feature-group archive), not inlined via .include "nic.s".
        StringAssert.Contains(inc, ".include \"nic.inc\"");
    }

    [TestMethod]
    public void EditUiExposesSharedDirtyWorkspaceIndicator()
    {
        string inc = File.ReadAllText(RepoPath("software", "runtime", "asm", "editui.inc"));
        string impl = File.ReadAllText(RepoPath("software", "runtime", "asm", "editui.s"));
        string docs = File.ReadAllText(RepoPath("docs", "assembly", "editui.md"));

        StringAssert.Contains(inc, ".global EDITUI_DIRTY");
        StringAssert.Contains(impl, "EDITUI_DIRTY:");
        StringAssert.Contains(impl, "STZ   EDITUI_DIRTY");
        StringAssert.Contains(impl, "LDA   EDITUI_DIRTY");
        StringAssert.Contains(docs, "EDITUI_DIRTY");
    }

    [TestMethod]
    public void EditUiExposesGeneralizedSaveUnderForDialogs()
    {
        string inc = File.ReadAllText(RepoPath("software", "runtime", "asm", "editui.inc"));
        string impl = File.ReadAllText(RepoPath("software", "runtime", "asm", "editui.s"));

        // The menu save/restore-under mechanism is generalized into reusable
        // rect-based entry points so dialogs can overlay without clearing.
        StringAssert.Contains(inc, ".global editui_save_under");
        StringAssert.Contains(inc, ".global editui_restore_under");
        StringAssert.Contains(inc, ".global EDITUI_MENU_SAVE_X");
        StringAssert.Contains(impl, "editui_save_under:");
        StringAssert.Contains(impl, "editui_restore_under:");
    }

    [TestMethod]
    public void EditUiStatusReplacesTextBeforeClearingItsStaleTail()
    {
        string source = File.ReadAllText(RepoPath("software", "runtime", "asm", "editui.s"));
        int start = source.IndexOf("editui_draw_status:", StringComparison.Ordinal);
        int end = source.IndexOf("editui_menu_open_hotkey:", start, StringComparison.Ordinal);
        string routine = source[start..end];

        int paint = routine.IndexOf("JSR   editui_print_marked", StringComparison.Ordinal);
        int clear = routine.IndexOf("JMP   vtext_clear_region", StringComparison.Ordinal);
        Assert.IsTrue(paint >= 0 && clear > paint,
            "Status updates must paint replacement text before clearing only its stale tail; clear-first visibly flickers.");
    }

    [TestMethod]
    public void EditUiMenuHotkeysMatchVisibleAmpersandMarkers()
    {
        string impl = File.ReadAllText(RepoPath("software", "runtime", "asm", "editui.s"));

        var labels = Regex.Matches(impl, @"(?m)^(editui_(?:menu_title|item)_[A-Za-z0-9_]+):\s*\n\s*\.byte\s+""([^""]+)"",0")
            .ToDictionary(m => m.Groups[1].Value, m => m.Groups[2].Value);
        var hotkeys = Regex.Matches(impl, @"\.byte\s+(?:EDITUI_CMD_[A-Z_]+|'.'),\s*'([^']+)',\s*<([^,]+),\s*>[^\r\n]+")
            .Cast<Match>()
            .Concat(Regex.Matches(impl, @"\.byte\s+'([^']+)',\s*\d+,\s*<([^,]+),\s*>[^\r\n]+").Cast<Match>());

        foreach (Match match in hotkeys)
        {
            string labelName = match.Groups[2].Value.Trim();
            Assert.IsTrue(labels.ContainsKey(labelName), $"Missing menu label {labelName}.");
            string label = labels[labelName];
            Assert.AreEqual(char.ToLowerInvariant(match.Groups[1].Value[0]), MarkedHotkey(label),
                $"{labelName} hotkey byte must match the visible '&' marker in \"{label}\".");
        }
    }

    [TestMethod]
    public void EditBufExposesSharedEditingEngine()
    {
        string inc = File.ReadAllText(RepoPath("software", "runtime", "asm", "editbuf.inc"));
        string impl = File.ReadAllText(RepoPath("software", "runtime", "asm", "editbuf.s"));

        // Public entry points.
        StringAssert.Contains(inc, ".global editbuf_run");
        StringAssert.Contains(inc, ".global editbuf_dialog3");
        StringAssert.Contains(impl, "editbuf_run:");
        StringAssert.Contains(impl, "editbuf_dialog3:");

        // Language-specific behavior arrives only through host hook vectors.
        StringAssert.Contains(inc, ".global EDITBUF_SAVE_VECL");
        StringAssert.Contains(inc, ".global EDITBUF_INDENT_VECL");
        StringAssert.Contains(inc, ".global EDITBUF_HILITE_VECL");
        StringAssert.Contains(impl, "JMP   (EDITBUF_SAVE_VECL)");
        StringAssert.Contains(impl, "JMP   (EDITBUF_INDENT_VECL)");
        StringAssert.Contains(impl, "JMP   (EDITBUF_HILITE_VECL)");

        // Each hook defaults to a no-op so the editor stands alone.
        StringAssert.Contains(impl, "editbuf_default_save:");
        StringAssert.Contains(impl, "editbuf_default_indent:");
        StringAssert.Contains(impl, "editbuf_default_hilite:");

        // Caller-owned buffer + dirty state are shared, not Logo-specific.
        StringAssert.Contains(inc, ".global EDITBUF_BUFL");
        StringAssert.Contains(inc, ".global EDITBUF_LENL");
    }

    [TestMethod]
    public void EditorModuleEditContractUsesSingleHookTableAbi()
    {
        string inc = File.ReadAllText(RepoPath("software", "runtime", "asm", "libeditor.inc"));
        IReadOnlyDictionary<string, int> constants = ParseCa65NumericConstants(inc);
        string editEx = "EDITOR_FN_EDIT" + "_EX";
        string profilePrefix = "EDITOR_EDIT" + "_PROFILE";

        AssertConstant("EDITOR_FN_EDIT", 0x00);
        AssertConstant("EDITOR_FN_EDIT_XRAM", 0x01);
        AssertConstant("EDITOR_XRAM_FLAG_HOOKS", 0x01);
        AssertConstant("EDITOR_FN_COUNT", 0x02);
        Assert.IsFalse(constants.ContainsKey(editEx), "The editor ABI must not keep a legacy extended edit function.");
        Assert.IsFalse(constants.Keys.Any(name => name.StartsWith(profilePrefix, StringComparison.Ordinal)), "Editor profiles must be removed; callers pass hook tables.");

        AssertConstant("EDITOR_HOOKS_TYPEL", 0);
        AssertConstant("EDITOR_HOOKS_TYPEH", 1);
        AssertConstant("EDITOR_HOOKS_STATUSL", 2);
        AssertConstant("EDITOR_HOOKS_STATUSH", 3);
        AssertConstant("EDITOR_HOOKS_SAVE_VECL", 4);
        AssertConstant("EDITOR_HOOKS_SAVE_VECH", 5);
        AssertConstant("EDITOR_HOOKS_INDENT_VECL", 6);
        AssertConstant("EDITOR_HOOKS_INDENT_VECH", 7);
        AssertConstant("EDITOR_HOOKS_HILITE_VECL", 8);
        AssertConstant("EDITOR_HOOKS_HILITE_VECH", 9);
        AssertConstant("EDITOR_HOOKS_MENU_VECL", 10);
        AssertConstant("EDITOR_HOOKS_MENU_VECH", 11);
        AssertConstant("EDITOR_HOOKS_COMMAND_VECL", 12);
        AssertConstant("EDITOR_HOOKS_COMMAND_VECH", 13);
        AssertConstant("EDITOR_HOOKS_CHANGED_VECL", 14);
        AssertConstant("EDITOR_HOOKS_CHANGED_VECH", 15);
        AssertConstant("EDITOR_HOOKS_SIZE", 16);
        AssertConstant("EDITOR_HOOK_ABI_HL_PTRL", 0x00AB);
        AssertConstant("EDITOR_HOOK_ABI_HL_PTRH", 0x00AC);
        AssertConstant("EDITOR_HOOK_ABI_HL_LEN", 0x055C);
        AssertConstant("EDITOR_HOOK_ABI_HL_COLORS", 0x055D);
        AssertConstant("EDITOR_HOOK_ABI_BUFL", 0x0430);
        AssertConstant("EDITOR_HOOK_ABI_LENL", 0x0438);
        AssertConstant("EDITOR_HOOK_ABI_TYPEL", 0x043C);
        AssertConstant("EDITOR_HOOK_ABI_STATUSL", 0x043E);
        AssertConstant("EDITOR_HOOK_ABI_SAVE_VECL", 0x0440);
        AssertConstant("EDITOR_HOOK_ABI_INDENT_VECL", 0x0442);
        AssertConstant("EDITOR_HOOK_ABI_HILITE_VECL", 0x0444);
        AssertConstant("EDITOR_HOOK_ABI_MENU_VECL", 0x0446);
        AssertConstant("EDITOR_HOOK_ABI_COMMAND_VECL", 0x0448);
        AssertConstant("EDITOR_HOOK_ABI_CHANGED_VECL", 0x044A);
        AssertConstant("EDITOR_HOOK_ABI_CURL", 0x044D);
        Assert.IsFalse(inc.Contains(editEx, StringComparison.Ordinal), "libeditor.inc must document only the single hook-table EDIT function.");
        Assert.IsFalse(inc.Contains(profilePrefix, StringComparison.Ordinal), "libeditor.inc must not expose profile-byte compatibility constants.");

        void AssertConstant(string name, int expected)
        {
            Assert.IsTrue(constants.TryGetValue(name, out int actual), $"{name} must be defined in libeditor.inc.");
            Assert.AreEqual(expected, actual, name);
        }
    }

    [TestMethod]
    public void EditorHookAbiConstantsMatchEditorMap()
    {
        string inc = File.ReadAllText(RepoPath("software", "runtime", "asm", "libeditor.inc"));
        string map = File.ReadAllText(RepoPath("software", "modules", "editor", "editor.map"));
        IReadOnlyDictionary<string, int> constants = ParseCa65NumericConstants(inc);

        AssertMapSymbol("EDITOR_HOOK_ABI_BUFL", "EDITBUF_BUFL");
        AssertMapSymbol("EDITOR_HOOK_ABI_LENL", "EDITBUF_LENL");
        AssertMapSymbol("EDITOR_HOOK_ABI_TYPEL", "EDITBUF_TYPEL");
        AssertMapSymbol("EDITOR_HOOK_ABI_STATUSL", "EDITBUF_STATUSL");
        AssertMapSymbol("EDITOR_HOOK_ABI_SAVE_VECL", "EDITBUF_SAVE_VECL");
        AssertMapSymbol("EDITOR_HOOK_ABI_INDENT_VECL", "EDITBUF_INDENT_VECL");
        AssertMapSymbol("EDITOR_HOOK_ABI_HILITE_VECL", "EDITBUF_HILITE_VECL");
        AssertMapSymbol("EDITOR_HOOK_ABI_MENU_VECL", "EDITBUF_MENU_VECL");
        AssertMapSymbol("EDITOR_HOOK_ABI_COMMAND_VECL", "EDITBUF_COMMAND_VECL");
        AssertMapSymbol("EDITOR_HOOK_ABI_CHANGED_VECL", "EDITBUF_CHANGED_VECL");
        AssertMapSymbol("EDITOR_HOOK_ABI_CURL", "EDITBUF_CURL");
        AssertMapSymbol("EDITOR_HOOK_ABI_HL_LEN", "EDITBUF_HL_LEN");
        AssertMapSymbol("EDITOR_HOOK_ABI_HL_COLORS", "EDITBUF_HL_COLORS");

        void AssertMapSymbol(string constantName, string symbolName)
        {
            Assert.IsTrue(constants.TryGetValue(constantName, out int expected), $"{constantName} must be defined.");
            Assert.AreEqual(expected, ParseMapSymbol(map, symbolName), symbolName);
        }
    }

    [TestMethod]
    public void NovaChessNetworkAdapterConstantsMatchServerChessAdapter()
    {
        string inc = File.ReadAllText(RepoPath("software", "examples", "novachess", "src", "nchess_net.inc"));
        IReadOnlyDictionary<string, int> constants = ParseCa65NumericConstants(inc);

        Assert.AreEqual(ChessGameAdapter.GameId, constants["NCHESS_GAME_ID"]);
        Assert.AreEqual(ChessGameAdapter.ProtocolMajor, constants["NCHESS_PROTO_MAJOR"]);
        Assert.AreEqual(ChessGameAdapter.ProtocolMinor, constants["NCHESS_PROTO_MINOR"]);
        Assert.AreEqual(ChessGameAdapter.ActionMove, constants["NCHESS_ACTION_MOVE"]);
        Assert.AreEqual(ChessGameAdapter.ActionResign, constants["NCHESS_ACTION_RESIGN"]);
        Assert.AreEqual(ChessGameAdapter.ActionOfferDraw, constants["NCHESS_ACTION_OFFER_DRAW"]);

        string impl = File.ReadAllText(RepoPath("software", "examples", "novachess", "src", "nchess_net.s"));
        foreach (string routine in new[]
        {
            "nchess_net_init",
            "nchess_net_build_hello",
            "nchess_net_build_opponent_list_request",
            "nchess_net_build_create_table",
            "nchess_net_build_join_table",
            "nchess_net_build_add_ai",
            "nchess_net_build_move",
            "nchess_net_parse_table_event",
            "nchess_net_parse_state_chunk"
        })
        {
            StringAssert.Contains(inc, $".global {routine}");
            StringAssert.Contains(impl, $".export {routine}");
        }
    }

    [TestMethod]
    public void VTextStateUsesLinkerStorageNotNvrMailbox()
    {
        string inc = File.ReadAllText(RepoPath("software", "runtime", "asm", "vtext.inc"));
        string impl = File.ReadAllText(RepoPath("software", "runtime", "asm", "vtext.s"));

        string[] stateSymbols =
        [
            "VTEXT_LEFT",
            "VTEXT_TOP",
            "VTEXT_WIDTH",
            "VTEXT_HEIGHT",
            "VTEXT_CURX",
            "VTEXT_CURY",
            "VTEXT_COLOR",
            "VTEXT_ATTR",
            "VTEXT_CHAR",
            "VTEXT_FLAGS",
            "VTEXT_TABLEL",
            "VTEXT_TABLEH",
            "VTEXT_REGION_ID"
        ];

        foreach (string symbol in stateSymbols)
        {
            AssertNoNvrAlias(inc, symbol);
            StringAssert.Contains(impl, $"{symbol}:");
        }

        StringAssert.Contains(impl, ".segment \"BSS\"");
    }

    [TestMethod]
    public void VTextExposeGfxSpacesIsPublicAndDocumented()
    {
        string inc = File.ReadAllText(RepoPath("software", "runtime", "asm", "vtext.inc"));
        string impl = File.ReadAllText(RepoPath("software", "runtime", "asm", "vtext.s"));

        StringAssert.Contains(inc, ".global vtext_expose_gfx_spaces_region");
        StringAssert.Contains(impl, ".export vtext_expose_gfx_spaces_region");
        StringAssert.Contains(impl, "; @label VTEXT.EXPOSE_GFX_SPACES_REGION");
        StringAssert.Contains(impl, "without");
        StringAssert.Contains(impl, "deleting non-space text");
    }

    [TestMethod]
    public void VTextExposeGfxSpacesPrimesVramReadLatch()
    {
        string impl = File.ReadAllText(RepoPath("software", "runtime", "asm", "vtext.s"));
        string routine = Slice(impl, "vtext_expose_gfx_spaces_region:", "; @label VTEXT.CLEAR_LINE");

        // VGC VRAM reads are latched. This routine scans the char plane while
        // temporarily switching to color/attr planes to style space cells, so
        // every return to the char plane must do a dummy read before comparing
        // the next cell. Otherwise a space can apply transparent style to the
        // following non-space character.
        int firstSelect = IndexOfOrFail(routine, "JSR   vtext_select_vram_addr_inc", "initial char select");
        int firstRead = IndexOfOrFail(routine[firstSelect..], "LDA   VGC_VRAM_DATA              ; prime VRAM read latch", "initial latch prime");
        int compareRead = IndexOfOrFail(routine[firstSelect..], "@cell_loop:\n      LDA   VGC_VRAM_DATA", "cell compare read");
        Assert.IsTrue(firstRead < compareRead, "Initial char-plane stream must be primed before comparing cells.");

        int textAttr = IndexOfOrFail(routine, "LDA   #VTEXT_PLANE_TEXTATTR", "text-attr plane switch");
        int secondSelect = IndexOfOrFail(routine[textAttr..], "JSR   vtext_select_vram_addr_inc", "resumed char select");
        int secondRead = IndexOfOrFail(routine[(textAttr + secondSelect)..], "LDA   VGC_VRAM_DATA              ; prime next-cell read after plane switch", "resumed latch prime");
        Assert.IsTrue(secondRead >= 0, "Resumed char-plane stream must be primed after styling a space cell.");

        static string Slice(string source, string startMarker, string endMarker)
        {
            int start = source.IndexOf(startMarker, StringComparison.Ordinal);
            Assert.IsTrue(start >= 0, $"Missing start marker {startMarker}.");
            int end = source.IndexOf(endMarker, start, StringComparison.Ordinal);
            Assert.IsTrue(end > start, $"Missing end marker {endMarker} after {startMarker}.");
            return source[start..end];
        }

        static int IndexOfOrFail(string source, string value, string name)
        {
            int index = source.IndexOf(value, StringComparison.Ordinal);
            Assert.IsTrue(index >= 0, $"{name} missing {value}.");
            return index;
        }
    }

    [TestMethod]
    public void VTextCommitsStylePlanesBeforeGlyphPlanes()
    {
        string impl = File.ReadAllText(RepoPath("software", "runtime", "asm", "vtext.s"));

        // VTEXT updates are visible to the raster as they happen. Style planes
        // must be committed before the char plane so clears, runs, and scrolls
        // never briefly expose old/default cell backgrounds.
        AssertPlaneOrder(
            SliceRoutine(impl, "vtext_write_current_cell:", "; ---------------------------------------------------------------------\n; Character output"),
            "vtext_write_current_cell");

        AssertPlaneOrder(
            SliceRoutine(impl, "vtext_put_run:", "; @label VTEXT.PUT_HEX_NIBBLE"),
            "vtext_put_run");

        AssertPlaneOrder(
            SliceRoutine(impl, "vtext_clear_region:", "; @label VTEXT.FILL_STYLE_REGION"),
            "vtext_clear_region");

        AssertPlaneOrder(
            SliceRoutine(impl, "vtext_clear_line:", "; @label VTEXT.SCROLL_UP"),
            "vtext_clear_line");

        string scroll = SliceRoutine(impl, "vtext_scroll_up:", "vtext_blt_fill_plane:");
        int copyChar = AssertPlaneOrder(scroll, "vtext_scroll_up copy");
        AssertPlaneOrder(
            scroll[(copyChar + "LDA   #VTEXT_PLANE_CHAR".Length)..],
            "vtext_scroll_up bottom-row clear");

        string scrollDown = File.ReadAllText(RepoPath("software", "runtime", "asm", "vtext_scroll_down.s"));
        AssertPlaneOrder(scrollDown, "vtext_scroll_down copy");
        StringAssert.Contains(scrollDown, "JSR   vtext_clear_region",
            "vtext_scroll_down must reuse the already style-ordered region clear.");

        static string SliceRoutine(string source, string startMarker, string endMarker)
        {
            int start = source.IndexOf(startMarker, StringComparison.Ordinal);
            Assert.IsTrue(start >= 0, $"Missing start marker {startMarker}.");
            int end = source.IndexOf(endMarker, start, StringComparison.Ordinal);
            Assert.IsTrue(end > start, $"Missing end marker {endMarker} after {startMarker}.");
            return source[start..end];
        }

        static int AssertPlaneOrder(string routine, string name)
        {
            int color = IndexOfOrFail(routine, "LDA   #VTEXT_PLANE_COLOR", name);
            int attr = IndexOfOrFail(routine, "LDA   #VTEXT_PLANE_TEXTATTR", name);
            int ch = IndexOfOrFail(routine, "LDA   #VTEXT_PLANE_CHAR", name);

            Assert.IsTrue(
                color < attr && attr < ch,
                $"{name} must write COLOR, then TEXTATTR, then CHAR.");

            return ch;
        }

        static int IndexOfOrFail(string source, string value, string name)
        {
            int index = source.IndexOf(value, StringComparison.Ordinal);
            Assert.IsTrue(index >= 0, $"{name} missing {value}.");
            return index;
        }
    }

    [TestMethod]
    public void VTextAutomaticScrollHookIsOptIn()
    {
        string inc = File.ReadAllText(RepoPath("software", "runtime", "asm", "vtext.inc"));
        string impl = File.ReadAllText(RepoPath("software", "runtime", "asm", "vtext.s"));

        StringAssert.Contains(inc, ".global VTEXT_SCROLL_HOOKL");
        StringAssert.Contains(inc, ".global VTEXT_SCROLL_HOOKH");
        StringAssert.Contains(inc, ".global vtext_set_scroll_hook");
        StringAssert.Contains(inc, ".global vtext_clear_scroll_hook");

        string advance = Slice(impl, "vtext_advance_line:", "; @label VTEXT.SET_SCROLL_HOOK");
        StringAssert.Contains(advance, "JSR   vtext_scroll_region");

        string dispatch = Slice(impl, "vtext_scroll_region:", "; ---------------------------------------------------------------------\n; Clears and scrolling");
        StringAssert.Contains(dispatch, "JMP   (VTEXT_SCROLL_HOOKL)");
        StringAssert.Contains(dispatch, "JMP   vtext_scroll_up");

        static string Slice(string source, string startMarker, string endMarker)
        {
            int start = source.IndexOf(startMarker, StringComparison.Ordinal);
            Assert.IsTrue(start >= 0, $"Missing start marker {startMarker}.");
            int end = source.IndexOf(endMarker, start, StringComparison.Ordinal);
            Assert.IsTrue(end > start, $"Missing end marker {endMarker} after {startMarker}.");
            return source[start..end];
        }
    }

    [TestMethod]
    public void VTextMixedExposesPixelPreciseGfxScroll()
    {
        string inc = File.ReadAllText(RepoPath("software", "runtime", "asm", "vtext.inc"));
        string mixed = File.ReadAllText(RepoPath("software", "runtime", "asm", "vtext_mixed.s"));

        // Mixed text/gfx runtimes sometimes have a logical pixel window that
        // is wider than the safe text-cell rectangle. The NDK must provide a
        // reusable gfx-plane scroll primitive so apps do not grow one-off
        // blitter code.
        string[] stateSymbols =
        [
            "VTEXT_GFX_LEFTL",
            "VTEXT_GFX_LEFTH",
            "VTEXT_GFX_TOP",
            "VTEXT_GFX_WIDTHL",
            "VTEXT_GFX_WIDTHH",
            "VTEXT_GFX_HEIGHT"
        ];

        StringAssert.Contains(inc, ".global vtext_scroll_gfx_pixels_up");
        StringAssert.Contains(inc, ".global vtext_fill_gfx_region");
        StringAssert.Contains(mixed, ".export vtext_scroll_gfx_pixels_up");
        StringAssert.Contains(mixed, ".export vtext_fill_gfx_region");
        StringAssert.Contains(mixed, "; @label VTEXT.SCROLL_GFX_PIXELS_UP");
        StringAssert.Contains(mixed, "; @label VTEXT.FILL_GFX_REGION");

        foreach (string symbol in stateSymbols)
        {
            StringAssert.Contains(inc, $".global {symbol}");
            StringAssert.Contains(mixed, $"{symbol}:");
        }

        string routine = Slice(mixed, "vtext_scroll_gfx_pixels_up:", "vtext_mixed_validate_region:");
        StringAssert.Contains(routine, "JSR   vtext_gfx_validate_region");
        StringAssert.Contains(routine, "JSR   vtext_mixed_wait_frame");
        StringAssert.Contains(routine, "JSR   vtext_gfx_pixels_up");

        // The cell-rectangle -> 4px-per-cell gfx-rectangle conversion now lives in
        // the shared vtext_mixed_cell_to_gfx_region helper (reused by the fill and
        // composite-scroll paths). Verify the conversion is present there and that
        // the fill routine drives it before filling.
        string cellToGfx = Slice(mixed, "vtext_mixed_cell_to_gfx_region:", "; @label VTEXT.SCROLL_MIXED_UP");
        StringAssert.Contains(cellToGfx, "STA   VTEXT_GFX_LEFTL");
        StringAssert.Contains(cellToGfx, "STA   VTEXT_GFX_TOP");
        StringAssert.Contains(cellToGfx, "STA   VTEXT_GFX_WIDTHL");
        StringAssert.Contains(cellToGfx, "STA   VTEXT_GFX_HEIGHT");

        string fillRoutine = Slice(mixed, "vtext_fill_gfx_region:", "; @label VTEXT.SCROLL_GFX_PIXELS_UP");
        StringAssert.Contains(fillRoutine, "JSR   vtext_mixed_validate_region");
        StringAssert.Contains(fillRoutine, "JSR   vtext_mixed_cell_to_gfx_region");
        StringAssert.Contains(fillRoutine, "JMP   vtext_gfx_fill");

        string implementation = Slice(mixed, "vtext_gfx_pixels_up:", "vtext_mixed_gfx_up:");
        StringAssert.Contains(implementation, "LDA   #BLT_SPACE_VGC_GFX");
        StringAssert.Contains(implementation, "STA   BLT_SRCSPACE");
        StringAssert.Contains(implementation, "STA   BLT_DSTSPACE");
        StringAssert.Contains(implementation, "LDA   #<VTEXT_GFX_ROW_PIXELS");
        StringAssert.Contains(implementation, "LDA   #>VTEXT_GFX_ROW_PIXELS");
        StringAssert.Contains(implementation, "JSR   blitter_start_copy");
        StringAssert.Contains(implementation, "JSR   blitter_fill");
        StringAssert.Contains(implementation, "CMP   #BLITTER_RESULT_OK");

        static string Slice(string source, string startMarker, string endMarker)
        {
            int start = source.IndexOf(startMarker, StringComparison.Ordinal);
            Assert.IsTrue(start >= 0, $"Missing start marker {startMarker}.");
            int end = source.IndexOf(endMarker, start, StringComparison.Ordinal);
            Assert.IsTrue(end > start, $"Missing end marker {endMarker} after {startMarker}.");
            return source[start..end];
        }
    }

    [TestMethod]
    public void VgcWaitCommandArmsBeforeAcceptingIdle()
    {
        string vgc = File.ReadAllText(RepoPath("software", "runtime", "asm", "vgc.s"));
        string wait = File.ReadAllText(RepoPath("software", "runtime", "asm", "vgc_wait.s"));
        string mixed = File.ReadAllText(RepoPath("software", "runtime", "asm", "vtext_mixed.s"));

        AssertArmedWait(vgc);
        AssertArmedWait(wait);

        string composite = Slice(mixed, "vtext_scroll_composite_up:", "; @label VTEXT.FILL_GFX_REGION");
        StringAssert.Contains(composite, "STA   VGC_CMD");
        StringAssert.Contains(composite, "LDY   #$20");
        StringAssert.Contains(composite, "@arm:");
        StringAssert.Contains(composite, "BNE   @arm");
        StringAssert.Contains(composite, "@wait:");

        static void AssertArmedWait(string source)
        {
            string routine = Slice(source, "vgc_wait_cmd:", ".endif");
            StringAssert.Contains(routine, "LDY   #$20");
            StringAssert.Contains(routine, "@arm:");
            StringAssert.Contains(routine, "DEY");
            StringAssert.Contains(routine, "BNE   @arm");
            StringAssert.Contains(routine, "@wait:");
        }

        static string Slice(string source, string startMarker, string endMarker)
        {
            int start = source.IndexOf(startMarker, StringComparison.Ordinal);
            Assert.IsTrue(start >= 0, $"Missing start marker {startMarker}.");
            int end = source.IndexOf(endMarker, start, StringComparison.Ordinal);
            Assert.IsTrue(end > start, $"Missing end marker {endMarker} after {startMarker}.");
            return source[start..end];
        }
    }

    [TestMethod]
    public void VSpriteRotateAbiUsesCallerOwnedBuffersAndStableRoutineSymbols()
    {
        string inc = File.ReadAllText(RepoPath("software", "runtime", "asm", "vsprite.inc"));
        string impl = File.ReadAllText(RepoPath("software", "runtime", "asm", "vsprite.s"));

        string[] rotateStateSymbols =
        [
            "VSPRITE_ORIGSPACE",
            "VSPRITE_ORIGADDRL",
            "VSPRITE_ORIGADDRM",
            "VSPRITE_ORIGADDRH",
            "VSPRITE_ORIGSTRL",
            "VSPRITE_ORIGSTRH",
            "VSPRITE_ROTSPACE",
            "VSPRITE_ROTADDRL",
            "VSPRITE_ROTADDRM",
            "VSPRITE_ROTADDRH",
            "VSPRITE_ROTSTRL",
            "VSPRITE_ROTSTRH",
            "VSPRITE_ROTANGLE"
        ];

        foreach (string symbol in rotateStateSymbols)
        {
            AssertNoNvrAlias(inc, symbol);
            StringAssert.Contains(inc, $".global {symbol}");
            StringAssert.Contains(impl, $"{symbol}:");
        }

        string[] routines =
        [
            "vsprite_rotate",
            "vsprite_gfx_rotate_blit",
            "vsprite_use_original",
            "vsprite_use_rotated"
        ];

        foreach (string routine in routines)
        {
            StringAssert.Contains(inc, $".global {routine}");
            StringAssert.Contains(impl, $".export {routine}");
        }

        StringAssert.Contains(impl, "LDA   #BLT_MODE_ROTATE");
        StringAssert.Contains(impl, "STA   BLT_ROTANGLE");
        StringAssert.Contains(impl, "JSR   vsprite_use_rotated");
        StringAssert.Contains(impl, "vsprite_gfx_rotate_blit:");
        StringAssert.Contains(impl, "JSR   vsprite_wait_frame");
        StringAssert.Contains(impl, "STZ   VSPRITE_FLAGS");
        StringAssert.Contains(impl, "JSR   vsprite_gfx_blit");
        StringAssert.Contains(impl, "LDA   VGC_FRAME");
    }

    [TestMethod]
    public void OverlayManagerUsesFixedHeaderAndLinkerStorage()
    {
        string inc = File.ReadAllText(RepoPath("software", "runtime", "asm", "overlay.inc"));
        string impl = File.ReadAllText(RepoPath("software", "runtime", "asm", "overlay.s"));
        IReadOnlyDictionary<string, int> constants = ParseCa65NumericConstants(inc);

        Assert.AreEqual(0x20, constants["OVL_HEADER_SIZE"]);
        Assert.AreEqual(0x01, constants["OVL_FORMAT_VERSION"]);
        Assert.AreEqual(0x01, constants["OVL_ABI_MAJOR"]);
        Assert.AreEqual(0x00, constants["OVL_HDR_MAGIC0"]);
        Assert.AreEqual(0x08, constants["OVL_HDR_LOADL"]);
        Assert.AreEqual(0x0A, constants["OVL_HDR_SIZEL"]);
        Assert.AreEqual(0x0E, constants["OVL_HDR_INITL"]);
        Assert.AreEqual(0x10, constants["OVL_HDR_MAINL"]);
        Assert.AreEqual(0x14, constants["OVL_HDR_UNLOADL"]);
        StringAssert.Contains(inc, "OVL_MAGIC0             = 'N'");

        string[] stateSymbols =
        [
            "OVL_NAMEPTR_L",
            "OVL_NAMEPTR_H",
            "OVL_NAMELEN",
            "OVL_LOADL",
            "OVL_LOADH",
            "OVL_MAXLENL",
            "OVL_MAXLENH",
            "OVL_RESULT",
            "OVL_LOADED",
            "OVL_ACTIVE_LOADL",
            "OVL_ACTIVE_LOADH",
            "OVL_ACTIVE_SIZEL",
            "OVL_ACTIVE_SIZEH",
            "OVL_ENTRY_INITL",
            "OVL_ENTRY_INITH",
            "OVL_ENTRY_MAINL",
            "OVL_ENTRY_MAINH",
            "OVL_ENTRY_TICKL",
            "OVL_ENTRY_TICKH",
            "OVL_ENTRY_UNLOADL",
            "OVL_ENTRY_UNLOADH"
        ];

        foreach (string symbol in stateSymbols)
        {
            AssertNoNvrAlias(inc, symbol);
            StringAssert.Contains(inc, $".global {symbol}");
            StringAssert.Contains(impl, $"{symbol}:");
        }

        string[] routines =
        [
            "overlay_ok",
            "overlay_set_error",
            "overlay_clear_active",
            "overlay_load_fixed",
            "overlay_unload",
            "overlay_call_init",
            "overlay_call_main",
            "overlay_call_tick"
        ];

        foreach (string routine in routines)
        {
            StringAssert.Contains(inc, $".global {routine}");
            StringAssert.Contains(impl, $".export {routine}");
        }

        StringAssert.Contains(impl, ".segment \"BSS\"");
        // Pager dependency (which transitively pulls fio.inc) is declared through
        // the header for linker resolution from nova.lib, not inlined via .include.
        StringAssert.Contains(inc, ".include \"pager.inc\"");
        StringAssert.Contains(impl, "PAGER_TARGET_RAM");
    }

    [TestMethod]
    public void XmcPrivateStateUsesLinkerStorageNotNvrMailbox()
    {
        string source = File.ReadAllText(RepoPath("software", "runtime", "asm", "xmc.s"));

        string[] privateStateSymbols =
        [
            "xmc_eidx",
            "xmc_dircur",
            "xmc_npgL",
            "xmc_npgH",
            "xmc_tmp",
            "xmc_tmp2",
            "xmc_pageL",
            "xmc_pageH",
            "xmc_run_startL",
            "xmc_run_startH",
            "xmc_run_lenL",
            "xmc_run_lenH",
            "xmc_rel_startL",
            "xmc_rel_startH",
            "xmc_rel_endL",
            "xmc_rel_endH"
        ];

        foreach (string symbol in privateStateSymbols)
        {
            AssertNoNvrAlias(source, symbol);
            Assert.IsTrue(
                Regex.IsMatch(source, $@"(?m)^{Regex.Escape(symbol)}:\s+\.res\s+1\b"),
                $"{symbol} should be allocated as private linker storage.");
        }

        StringAssert.Contains(source, ".segment \"BSS\"");
    }

    [TestMethod]
    public void TweenStateUsesLinkerStorageAndStableRoutineAbi()
    {
        string inc = File.ReadAllText(RepoPath("software", "runtime", "asm", "tween.inc"));
        string impl = File.ReadAllText(RepoPath("software", "runtime", "asm", "tween.s"));

        string[] stateSymbols =
        [
            "TWEEN_STARTL",
            "TWEEN_STARTH",
            "TWEEN_ENDL",
            "TWEEN_ENDH",
            "TWEEN_DURATION",
            "TWEEN_FRAME",
            "TWEEN_MODE",
            "TWEEN_VALUEL",
            "TWEEN_VALUEH",
            "TWEEN_DONE",
            "TWEEN_PROGRESS",
            "TWEEN_EASE"
        ];

        foreach (string symbol in stateSymbols)
        {
            AssertNoNvrAlias(inc, symbol);
            StringAssert.Contains(inc, $".global {symbol}");
            StringAssert.Contains(impl, $"{symbol}:");
        }

        string[] routines =
        [
            "tween_begin",
            "tween_eval",
            "tween_eval_linear",
            "tween_eval_ease_in",
            "tween_eval_ease_out",
            "tween_eval_ease_in_out",
            "tween_step",
            "tween_step_linear",
            "tween_step_ease_in",
            "tween_step_ease_out",
            "tween_step_ease_in_out"
        ];

        foreach (string routine in routines)
        {
            StringAssert.Contains(inc, $".global {routine}");
            StringAssert.Contains(impl, $".export {routine}");
        }

        StringAssert.Contains(impl, ".segment \"BSS\"");
        StringAssert.Contains(inc, "TWEEN_MODE_LINEAR");
        StringAssert.Contains(inc, "TWEEN_MODE_EASE_IN");
        StringAssert.Contains(inc, "TWEEN_MODE_EASE_OUT");
        StringAssert.Contains(inc, "TWEEN_MODE_EASE_IN_OUT");
    }

    [TestMethod]
    public void MetaSpriteStateUsesLinkerStorageAndStableRoutineAbi()
    {
        string inc = File.ReadAllText(RepoPath("software", "runtime", "asm", "msprite.inc"));
        string impl = File.ReadAllText(RepoPath("software", "runtime", "asm", "msprite.s"));
        IReadOnlyDictionary<string, int> constants = ParseCa65NumericConstants(inc);

        Assert.AreEqual(8, constants["MSPRITE_MAX_OBJECTS"]);
        Assert.AreEqual(0, constants["MSPRITE_RESULT_OK"]);
        Assert.AreEqual(0xFF, constants["MSPRITE_INVALID_HANDLE"]);
        Assert.AreEqual(0x80, constants["MSPRITE_OBJ_FLAG_ACTIVE"]);
        Assert.AreEqual(0x40, constants["MSPRITE_OBJ_FLAG_VISIBLE"]);
        Assert.AreEqual(0x20, constants["MSPRITE_OBJ_FLAG_DIRTY"]);

        string[] stateSymbols =
        [
            "MSPRITE_DESC_L",
            "MSPRITE_DESC_H",
            "MSPRITE_ANIM_L",
            "MSPRITE_ANIM_H",
            "MSPRITE_RESULT"
        ];

        foreach (string symbol in stateSymbols)
        {
            AssertNoNvrAlias(inc, symbol);
            StringAssert.Contains(inc, $".global {symbol}");
            StringAssert.Contains(impl, $"{symbol}:");
        }

        string[] routines =
        [
            "msprite_init",
            "msprite_spawn",
            "msprite_destroy",
            "msprite_show",
            "msprite_hide",
            "msprite_set_pos",
            "msprite_set_frame",
            "msprite_set_anim",
            "msprite_set_priority",
            "msprite_set_transcolor",
            "msprite_tick",
            "msprite_commit",
            "msprite_commit_one"
        ];

        foreach (string routine in routines)
        {
            StringAssert.Contains(inc, $".global {routine}");
            StringAssert.Contains(impl, $".export {routine}");
        }

        StringAssert.Contains(impl, ".segment \"BSS\"");
        // Sprite dependency is declared through the header for linker resolution
        // from nova.lib, not inlined via .include "sprite.s".
        StringAssert.Contains(inc, ".include \"sprite.inc\"");
    }

    [TestMethod]
    public void AnimationStateUsesLinkerStorageAndStableRoutineAbi()
    {
        string inc = File.ReadAllText(RepoPath("software", "runtime", "asm", "anim.inc"));
        string impl = File.ReadAllText(RepoPath("software", "runtime", "asm", "anim.s"));
        IReadOnlyDictionary<string, int> constants = ParseCa65NumericConstants(inc);

        Assert.AreEqual(8, constants["ANIM_MAX_TRACKS"]);
        Assert.AreEqual(128, constants["ANIM_SHAPE_BYTES"]);
        Assert.AreEqual(0xFF, constants["ANIM_INVALID_HANDLE"]);
        Assert.AreEqual(0, constants["ANIM_TARGET_SPRITE"]);
        Assert.AreEqual(1, constants["ANIM_TARGET_MSPRITE"]);
        Assert.AreEqual(0, constants["ANIM_DESC_FRAME_COUNT"]);
        Assert.AreEqual(1, constants["ANIM_DESC_TICKS"]);
        Assert.AreEqual(2, constants["ANIM_DESC_FLAGS"]);
        Assert.AreEqual(3, constants["ANIM_DESC_FIRST"]);
        Assert.AreEqual(4, constants["ANIM_DESC_STRIDE"]);
        Assert.AreEqual(6, constants["ANIM_DESC_FRAMES"]);
        Assert.AreEqual(0x01, constants["ANIM_DESC_LOOP"]);
        Assert.AreEqual(0x04, constants["ANIM_DESC_FRAME_TABLE"]);
        Assert.AreEqual(0x08, constants["ANIM_DESC_DEFER_COMMIT"]);

        string[] stateSymbols =
        [
            "ANIM_DESC_L",
            "ANIM_DESC_H",
            "ANIM_TARGET",
            "ANIM_TARGET_TYPE",
            "ANIM_RESULT",
            "ANIM_SHAPE",
            "ANIM_COUNT",
            "ANIM_XADDRL",
            "ANIM_XADDRM",
            "ANIM_XADDRH",
            "ANIM_FILEL",
            "ANIM_FILEM",
            "ANIM_FILEH",
            "ANIM_NAMELEN",
            "ANIM_NAMEPTR_L",
            "ANIM_NAMEPTR_H"
        ];

        foreach (string symbol in stateSymbols)
        {
            AssertNoNvrAlias(inc, symbol);
            StringAssert.Contains(inc, $".global {symbol}");
            StringAssert.Contains(impl, $"{symbol}:");
        }

        string[] routines =
        [
            "anim_init",
            "anim_start",
            "anim_stop",
            "anim_tick",
            "anim_tick_one",
            "anim_set_frame",
            "anim_load_xram_shapes",
            "anim_load_disk_shapes"
        ];

        foreach (string routine in routines)
        {
            StringAssert.Contains(inc, $".global {routine}");
            StringAssert.Contains(impl, $".export {routine}");
        }

        StringAssert.Contains(impl, ".segment \"BSS\"");
        // Dependencies (dma, msprite, pager→fio) are declared through the header for
        // linker resolution from nova.lib, not inlined via .include "*.s".
        StringAssert.Contains(inc, ".include \"dma.inc\"");
        StringAssert.Contains(inc, ".include \"msprite.inc\"");
        StringAssert.Contains(inc, ".include \"pager.inc\"");
        StringAssert.Contains(impl, "DMA_SPACE_XRAM");
        StringAssert.Contains(impl, "DMA_SPACE_VGC_SPRITE");
        StringAssert.Contains(impl, "PAGER_TARGET_VGC");
    }

    [TestMethod]
    public void ExtensionRomCodeSegmentUsesLinkerPlacement()
    {
        string source = File.ReadAllText(RepoPath("software", "languages", "ehbasic", "extension.s"));
        string config = File.ReadAllText(RepoPath("software", "languages", "ehbasic", "extension.cfg"));

        Assert.IsFalse(
            Regex.IsMatch(source, @"(?im)^\s*\.org\s+\$C000\b"),
            "Extension CODE must not pin .org $C000; it breaks linker-owned BSS relocation.");
        StringAssert.Contains(config, "BSS:");
    }

    private static void AssertNoNvrAlias(string source, string symbol)
    {
        Assert.IsFalse(
            Regex.IsMatch(source, $@"(?m)^\s*{Regex.Escape(symbol)}\s*=\s*NVR\d[HL]?\b"),
            $"{symbol} must not alias an NVR pseudo-register.");
    }

    private static void AssertCompleteNobj(ReadOnlySpan<byte> objectFile, int member)
    {
        Assert.IsTrue(objectFile.Length >= 20, $"NOBJ member {member} is shorter than its header.");
        Assert.IsTrue(objectFile[..4].SequenceEqual("NOBJ"u8), $"NOBJ member {member} has bad magic.");
        Assert.AreEqual(2, objectFile[4], $"NOBJ member {member} version");

        int sectionCount = objectFile[6];
        int symbolCount = objectFile[10] | objectFile[11] << 8;
        int symbolOffset = objectFile[12] | objectFile[13] << 8;
        int relocationCount = objectFile[14] | objectFile[15] << 8;
        int relocationOffset = objectFile[16] | objectFile[17] << 8;
        int cursor = 20;
        var sectionSizes = new List<int>(sectionCount);

        for (int section = 0; section < sectionCount; section++)
        {
            Assert.IsTrue(cursor + 8 <= objectFile.Length, $"NOBJ member {member} section {section} is truncated.");
            int nameLength = objectFile[cursor + 2];
            int size = objectFile[cursor + 4] | objectFile[cursor + 5] << 8;
            int dataLength = objectFile[cursor + 6] | objectFile[cursor + 7] << 8;
            Assert.IsTrue(dataLength <= size, $"NOBJ member {member} section {section} data exceeds memory size.");
            cursor += 8 + nameLength + dataLength;
            Assert.IsTrue(cursor <= objectFile.Length, $"NOBJ member {member} section {section} overruns the object.");
            sectionSizes.Add(size);
        }
        Assert.AreEqual(symbolOffset, cursor, $"NOBJ member {member} symbol-table offset");

        for (int symbol = 0; symbol < symbolCount; symbol++)
        {
            Assert.IsTrue(cursor + 5 <= objectFile.Length, $"NOBJ member {member} symbol {symbol} is truncated.");
            cursor += 5 + objectFile[cursor + 4];
            Assert.IsTrue(cursor <= objectFile.Length, $"NOBJ member {member} symbol {symbol} overruns the object.");
        }
        Assert.AreEqual(relocationOffset, cursor, $"NOBJ member {member} relocation-table offset");
        Assert.AreEqual(objectFile.Length, relocationOffset + relocationCount * 8,
            $"NOBJ member {member} relocation count must describe the complete table.");

        for (int relocation = 0; relocation < relocationCount; relocation++)
        {
            int record = relocationOffset + relocation * 8;
            int section = objectFile[record];
            int type = objectFile[record + 1];
            int patch = objectFile[record + 2] | objectFile[record + 3] << 8;
            int symbol = objectFile[record + 4] | objectFile[record + 5] << 8;
            Assert.IsTrue(section < sectionCount, $"NOBJ member {member} relocation {relocation} has a bad section.");
            Assert.IsTrue(symbol < symbolCount, $"NOBJ member {member} relocation {relocation} has a bad symbol.");
            int width = type == 1 ? 2 : 1;
            Assert.IsTrue(patch + width <= sectionSizes[section],
                $"NOBJ member {member} relocation {relocation} overruns its section.");
        }
    }

    private static char MarkedHotkey(string label)
    {
        int marker = label.IndexOf('&');
        Assert.IsTrue(marker >= 0 && marker + 1 < label.Length, $"Menu label must mark a hotkey: {label}");
        return char.ToLowerInvariant(label[marker + 1]);
    }

    private static IReadOnlyDictionary<string, int> ParseCa65NumericConstants(string source)
    {
        var constants = new Dictionary<string, int>(StringComparer.Ordinal);
        foreach (Match match in Regex.Matches(source, @"(?m)^\s*([A-Z0-9_]+)\s*=\s*(\$[0-9A-Fa-f]+|[0-9]+)\b"))
        {
            string value = match.Groups[2].Value;
            constants[match.Groups[1].Value] = value.StartsWith('$')
                ? int.Parse(value[1..], NumberStyles.HexNumber, CultureInfo.InvariantCulture)
                : int.Parse(value, CultureInfo.InvariantCulture);
        }

        return constants;
    }

    private static int ParseMapSymbol(string map, string symbol)
    {
        var match = Regex.Match(map, $@"\b{Regex.Escape(symbol)}\s+([0-9A-Fa-f]{{6}})\s+R");
        Assert.IsTrue(match.Success, $"Could not find {symbol} in linker map.");
        return Convert.ToInt32(match.Groups[1].Value, 16);
    }

    private static string RepoPath(params string[] parts)
    {
        string root = Path.GetFullPath(Path.Combine(
            AppContext.BaseDirectory,
            "..",
            "..",
            "..",
            ".."));

        return Path.Combine([root, .. parts]);
    }
}
