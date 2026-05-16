using System;
using System.Collections.Generic;
using System.Globalization;
using System.IO;
using System.Text.RegularExpressions;
using e6502.GameServer.Protocol;
using e6502.GameServer.Server;
using Microsoft.VisualStudio.TestTools.UnitTesting;

namespace e6502UnitTests;

[TestClass]
public class RuntimeLibraryAbiTests
{
    [TestMethod]
    public void GameServerAssemblyProtocolConstantsMatchServer()
    {
        string inc = File.ReadAllText(RepoPath("runtime", "asm", "gameserver.inc"));
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
        string inc = File.ReadAllText(RepoPath("runtime", "asm", "gameserver.inc"));
        string impl = File.ReadAllText(RepoPath("runtime", "asm", "gameserver.s"));

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
        StringAssert.Contains(impl, ".include \"nic.s\"");
    }

    [TestMethod]
    public void NovaChessNetworkAdapterConstantsMatchServerChessAdapter()
    {
        string inc = File.ReadAllText(RepoPath("examples", "novachess", "src", "nchess_net.inc"));
        IReadOnlyDictionary<string, int> constants = ParseCa65NumericConstants(inc);

        Assert.AreEqual(ChessGameAdapter.GameId, constants["NCHESS_GAME_ID"]);
        Assert.AreEqual(ChessGameAdapter.ProtocolMajor, constants["NCHESS_PROTO_MAJOR"]);
        Assert.AreEqual(ChessGameAdapter.ProtocolMinor, constants["NCHESS_PROTO_MINOR"]);
        Assert.AreEqual(ChessGameAdapter.ActionMove, constants["NCHESS_ACTION_MOVE"]);
        Assert.AreEqual(ChessGameAdapter.ActionResign, constants["NCHESS_ACTION_RESIGN"]);
        Assert.AreEqual(ChessGameAdapter.ActionOfferDraw, constants["NCHESS_ACTION_OFFER_DRAW"]);

        string impl = File.ReadAllText(RepoPath("examples", "novachess", "src", "nchess_net.s"));
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
        string inc = File.ReadAllText(RepoPath("runtime", "asm", "vtext.inc"));
        string impl = File.ReadAllText(RepoPath("runtime", "asm", "vtext.s"));

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
    public void OverlayManagerUsesFixedHeaderAndLinkerStorage()
    {
        string inc = File.ReadAllText(RepoPath("runtime", "asm", "overlay.inc"));
        string impl = File.ReadAllText(RepoPath("runtime", "asm", "overlay.s"));
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
        StringAssert.Contains(impl, ".include \"fio.s\"");
        StringAssert.Contains(impl, ".include \"pager.s\"");
        StringAssert.Contains(impl, "PAGER_TARGET_RAM");
    }

    [TestMethod]
    public void XmcPrivateStateUsesLinkerStorageNotNvrMailbox()
    {
        string source = File.ReadAllText(RepoPath("runtime", "asm", "xmc.s"));

        string[] privateStateSymbols =
        [
            "xmc_eidx",
            "xmc_dircur",
            "xmc_npgL",
            "xmc_npgH",
            "xmc_tmp",
            "xmc_tmp2"
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
        string inc = File.ReadAllText(RepoPath("runtime", "asm", "tween.inc"));
        string impl = File.ReadAllText(RepoPath("runtime", "asm", "tween.s"));

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
        string inc = File.ReadAllText(RepoPath("runtime", "asm", "msprite.inc"));
        string impl = File.ReadAllText(RepoPath("runtime", "asm", "msprite.s"));
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
        StringAssert.Contains(impl, ".include \"sprite.s\"");
    }

    [TestMethod]
    public void AnimationStateUsesLinkerStorageAndStableRoutineAbi()
    {
        string inc = File.ReadAllText(RepoPath("runtime", "asm", "anim.inc"));
        string impl = File.ReadAllText(RepoPath("runtime", "asm", "anim.s"));
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
        StringAssert.Contains(impl, ".include \"dma.s\"");
        StringAssert.Contains(impl, ".include \"fio.s\"");
        StringAssert.Contains(impl, ".include \"pager.s\"");
        StringAssert.Contains(impl, ".include \"msprite.s\"");
        StringAssert.Contains(impl, "DMA_SPACE_XRAM");
        StringAssert.Contains(impl, "DMA_SPACE_VGC_SPRITE");
        StringAssert.Contains(impl, "PAGER_TARGET_VGC");
    }

    [TestMethod]
    public void ExtensionRomCodeSegmentUsesLinkerPlacement()
    {
        string source = File.ReadAllText(RepoPath("ehbasic", "extension.s"));
        string config = File.ReadAllText(RepoPath("ehbasic", "extension.cfg"));

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
