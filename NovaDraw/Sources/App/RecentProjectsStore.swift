import Foundation

struct RecentProject: Identifiable, Equatable {
    let url: URL

    var id: String {
        url.standardizedFileURL.path
    }

    var title: String {
        url.deletingPathExtension().lastPathComponent
    }
}

enum RecentProjectsStore {
    static let defaultsKey = "NovaDrawRecentProjectURLs"
    static let maximumCount = 10

    static func load(defaults: UserDefaults = .standard) -> [URL] {
        guard let paths = defaults.stringArray(forKey: defaultsKey) else { return [] }
        return unique(paths.map { URL(fileURLWithPath: $0).standardizedFileURL })
    }

    static func save(_ urls: [URL], defaults: UserDefaults = .standard) {
        let paths = unique(urls).prefix(maximumCount).map(\.path)
        defaults.set(Array(paths), forKey: defaultsKey)
    }

    static func adding(_ url: URL, to urls: [URL]) -> [URL] {
        let normalizedURL = url.standardizedFileURL
        return Array(unique([normalizedURL] + urls).prefix(maximumCount))
    }

    static func removing(_ url: URL, from urls: [URL]) -> [URL] {
        let path = url.standardizedFileURL.path
        return urls.filter { $0.standardizedFileURL.path != path }
    }

    private static func unique(_ urls: [URL]) -> [URL] {
        var seen = Set<String>()
        var result: [URL] = []
        for url in urls {
            let normalizedURL = url.standardizedFileURL
            let path = normalizedURL.path
            guard seen.insert(path).inserted else { continue }
            result.append(normalizedURL)
        }
        return result
    }
}
