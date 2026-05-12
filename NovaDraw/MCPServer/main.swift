import Foundation
import MCP
import NovaDrawMCPKit

private let runner = NovaDrawMCPToolRunner()

private let tools = NovaDrawMCPToolRunner.toolDefinitions().map { definition in
    Tool(
        name: definition["name"] as? String ?? "unknown",
        description: definition["description"] as? String,
        inputSchema: value(from: definition["inputSchema"] ?? [:]),
        annotations: .init(
            readOnlyHint: (definition["name"] as? String)?.contains("inspect") == true
                || (definition["name"] as? String)?.contains("preview") == true,
            destructiveHint: false,
            idempotentHint: nil,
            openWorldHint: false
        )
    )
}

let server = Server(
    name: "NovaDrawMCP",
    version: "0.1.0",
    capabilities: .init(
        tools: .init(listChanged: false)
    )
)

await server.withMethodHandler(ListTools.self) { _ in
    .init(tools: tools)
}

await server.withMethodHandler(CallTool.self) { params in
    do {
        let result = try runner.call(
            toolName: params.name,
            arguments: (params.arguments ?? [:]).mapValues(any(from:))
        )
        return .init(
            content: [.text(text: result, annotations: nil, _meta: nil)],
            isError: false
        )
    } catch {
        return .init(
            content: [.text(text: error.localizedDescription, annotations: nil, _meta: nil)],
            isError: true
        )
    }
}

let transport = StdioTransport()
try await server.start(transport: transport)
await server.waitUntilCompleted()

private func value(from any: Any) -> Value {
    switch any {
    case let value as Value:
        return value
    case let value as String:
        return .string(value)
    case let value as Bool:
        return .bool(value)
    case let value as Int:
        return .int(value)
    case let value as Double:
        return .double(value)
    case let value as [Any]:
        return .array(value.map(value(from:)))
    case let value as [String: Any]:
        return .object(value.mapValues(value(from:)))
    default:
        return .null
    }
}

private func any(from value: Value) -> Any {
    switch value {
    case .null:
        return NSNull()
    case .bool(let bool):
        return bool
    case .int(let int):
        return int
    case .double(let double):
        return double
    case .string(let string):
        return string
    case .data(_, let data):
        return data.base64EncodedString()
    case .array(let values):
        return values.map(any(from:))
    case .object(let values):
        return values.mapValues(any(from:))
    }
}
