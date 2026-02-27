FROM mcr.microsoft.com/dotnet/sdk:10.0

RUN apt-get update -qq && \
    apt-get install -y -qq --no-install-recommends \
      cc65 \
      make \
      pandoc \
      texlive-latex-extra \
      texlive-fonts-recommended \
      texlive-latex-recommended \
      latexmk \
      python3-yaml \
      git \
      curl && \
    apt-get clean && \
    rm -rf /var/lib/apt/lists/*

# Install GitHub CLI
RUN curl -fsSL https://cli.github.com/packages/githubcli-archive-keyring.gpg -o /usr/share/keyrings/githubcli-archive-keyring.gpg && \
    echo "deb [arch=$(dpkg --print-architecture) signed-by=/usr/share/keyrings/githubcli-archive-keyring.gpg] https://cli.github.com/packages stable main" > /etc/apt/sources.list.d/github-cli.list && \
    apt-get update -qq && \
    apt-get install -y -qq --no-install-recommends gh && \
    apt-get clean && \
    rm -rf /var/lib/apt/lists/*

# Install cocogitto from GitHub release
RUN curl -fsSL https://github.com/cocogitto/cocogitto/releases/download/6.5.0/cocogitto-6.5.0-x86_64-unknown-linux-musl.tar.gz \
      | tar -xz --strip-components=1 -C /usr/local/bin x86_64-unknown-linux-musl/cog
