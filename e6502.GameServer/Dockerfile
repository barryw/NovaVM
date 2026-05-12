# syntax=docker/dockerfile:1

FROM mcr.microsoft.com/dotnet/sdk:10.0 AS build
WORKDIR /src

COPY Directory.Build.props ./
COPY e6502.GameServer/e6502.GameServer.csproj e6502.GameServer/
RUN dotnet restore e6502.GameServer/e6502.GameServer.csproj

COPY e6502.GameServer/ e6502.GameServer/
RUN dotnet publish e6502.GameServer/e6502.GameServer.csproj \
    -c Release \
    --no-restore \
    -o /app/publish \
    /p:UseAppHost=false

FROM mcr.microsoft.com/dotnet/runtime:10.0 AS runtime
WORKDIR /app

RUN apt-get update \
    && apt-get install -y --no-install-recommends stockfish \
    && rm -rf /var/lib/apt/lists/*

ENV NOVA_GAME_PORT=6503 \
    STOCKFISH_PATH=/usr/games/stockfish \
    STOCKFISH_MOVETIME_MS=500 \
    DOTNET_EnableDiagnostics=0

EXPOSE 6503

COPY --from=build /app/publish ./
ENTRYPOINT ["dotnet", "e6502.GameServer.dll"]
