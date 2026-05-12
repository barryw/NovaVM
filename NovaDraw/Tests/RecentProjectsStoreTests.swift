import Foundation
import Testing
@testable import NovaDraw

@Suite struct RecentProjectsStoreTests {
    @Test func addingMovesExistingProjectToTopAndDeduplicates() {
        let first = URL(fileURLWithPath: "/tmp/first.novadraw")
        let second = URL(fileURLWithPath: "/tmp/second.novadraw")

        let urls = RecentProjectsStore.adding(first, to: [second, first])

        #expect(urls == [first.standardizedFileURL, second.standardizedFileURL])
    }

    @Test func addingCapsRecentProjects() {
        let existing = (0..<RecentProjectsStore.maximumCount).map {
            URL(fileURLWithPath: "/tmp/project-\($0).novadraw")
        }
        let newest = URL(fileURLWithPath: "/tmp/newest.novadraw")

        let urls = RecentProjectsStore.adding(newest, to: existing)

        #expect(urls.count == RecentProjectsStore.maximumCount)
        #expect(urls.first == newest.standardizedFileURL)
        #expect(urls.last == existing[RecentProjectsStore.maximumCount - 2].standardizedFileURL)
    }

    @Test func loadAndSaveRoundTripThroughUserDefaults() throws {
        let defaults = try #require(UserDefaults(suiteName: "NovaDrawRecentProjectsStoreTests-\(UUID().uuidString)"))
        defer { defaults.removeObject(forKey: RecentProjectsStore.defaultsKey) }
        let urls = [
            URL(fileURLWithPath: "/tmp/one.novadraw"),
            URL(fileURLWithPath: "/tmp/two.novadraw"),
        ]

        RecentProjectsStore.save(urls, defaults: defaults)

        #expect(RecentProjectsStore.load(defaults: defaults) == urls.map(\.standardizedFileURL))
    }
}
