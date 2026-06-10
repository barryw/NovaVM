using System.IO;
using System.Linq;
using System.Text;
using e6502.Storage;
using Microsoft.VisualStudio.TestTools.UnitTesting;

namespace e6502UnitTests;

[TestClass]
public class ForthDiskLayoutTests
{
    private static string TempNdi() =>
        Path.Combine(Path.GetTempPath(), $"forth-{System.Guid.NewGuid():N}.ndi");

    [TestMethod]
    public void ForthLibraryDisk_UsesStandardLayoutAndForthFileType()
    {
        string path = TempNdi();
        try
        {
            CreateForthLibraryDisk(path);

            using var image = NdiImage.Open(path);
            ushort forthDir = FindDirectory(image, 0xFFFF, "forth");
            ushort libDir = FindDirectory(image, forthDir, "lib");
            ushort novaDir = FindDirectory(image, libDir, "nova");

            var core = image.ListDirectory(libDir).Single(e => e.Filename == "core.4th");
            Assert.AreEqual(NdiFileType.Forth, core.FileType,
                "core.4th is the boot library source; storing it as BIN would make a standard INCLUDED path ambiguous.");

            var autoexec = image.ListDirectory(forthDir).Single(e => e.Filename == "autoexec.4th");
            Assert.AreEqual(NdiFileType.Forth, autoexec.FileType,
                "autoexec.4th is the boot policy source; users must be able to edit it as plain Forth text.");

            var vgc = image.ListDirectory(novaDir).Single(e => e.Filename == "vgc.4th");
            Assert.AreEqual(NdiFileType.Forth, vgc.FileType,
                "Nova hardware libraries should be ordinary includable Forth source files.");

            string coreText = Encoding.ASCII.GetString(image.ReadFile("core.4th", libDir));
            StringAssert.Contains(coreText, "NovaForth core library");
        }
        finally
        {
            File.Delete(path);
        }
    }

    private static void CreateForthLibraryDisk(string path)
    {
        NdiImage.CreateFormatted(path, "FORTH", 800);
        using var image = NdiImage.Open(path);

        ushort forthDir = (ushort)image.MakeDirectory("forth", 0xFFFF);
        ushort libDir = (ushort)image.MakeDirectory("lib", forthDir);
        ushort novaDir = (ushort)image.MakeDirectory("nova", libDir);
        _ = image.MakeDirectory("compat", libDir);

        image.WriteFile("core.4th", NdiFileType.Forth, libDir,
            Encoding.ASCII.GetBytes("\\ NovaForth core library\n"));
        image.WriteFile("autoexec.4th", NdiFileType.Forth, forthDir,
            Encoding.ASCII.GetBytes("INCLUDE forth/lib/core-ext.4th\n"));
        image.WriteFile("site.4th", NdiFileType.Forth, forthDir,
            Encoding.ASCII.GetBytes("\\ Site-local startup hooks\n"));
        image.WriteFile("core-ext.4th", NdiFileType.Forth, libDir,
            Encoding.ASCII.GetBytes("\\ NovaForth core extensions\n"));
        image.WriteFile("vgc.4th", NdiFileType.Forth, novaDir,
            Encoding.ASCII.GetBytes("HEX A000 CONSTANT VGC-BASE DECIMAL\n"));
    }

    private static ushort FindDirectory(NdiImage image, ushort parent, string name)
    {
        var entry = image.ListDirectory(parent).Single(e => e.IsDirectory && e.Filename == name);
        return (ushort)entry.Index;
    }
}
