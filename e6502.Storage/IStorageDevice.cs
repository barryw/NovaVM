namespace e6502.Storage;

public record StorageDirEntry(string Filename, bool IsDirectory, NdiFileType FileType, int SizeBytes);

public interface IStorageDevice
{
    string Prefix { get; }
    string CurrentDirectory { get; set; }
    bool IsMounted { get; }
    byte[] Load(string name, string ext);
    void Save(string name, byte[] data, string ext);
    void Delete(string name, string ext);
    StorageDirEntry[] ListDirectory(string? path);
    void MakeDirectory(string name);
    void RemoveDirectory(string name);
    void Format(string label, int sizeKB);
    void Mount(string imagePath);
    void Unmount();
    bool FileExists(string name, string ext);
}
