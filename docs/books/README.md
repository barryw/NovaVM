# Nova Books

This directory is the canonical home for printable Nova documentation books:
book-scale manuals and references that build PDF output through LaTeX or a
Pandoc-to-LaTeX pipeline. It is not for ordinary README files, small notes, or
single-topic reference pages.

Each book has its own subdirectory and owns its front matter, chapters,
generated content, assets, build files, and PDF output. Shared visual style and
build conventions live under `common/`; `common/` is support code, not a book.

Current books:

| Directory | Book |
|---|---|
| `basic-user-guide/` | NovaBASIC User Guide |
| `mapping-the-novavm/` | Mapping the NovaVM |
| `ndk-reference/` | Nova NDK Reference |
| `nova-cli-guide/` | Nova CLI Guide |
| `nova-fun-n-games/` | Nova Fun & Games |

Build all books:

```sh
make -C docs/books
```

Build one book:

```sh
make -C docs/books/ndk-reference
```

Do not add book source outside this directory. If a document becomes a book,
move its source here and remove the old source path. If a document cannot stand
as a printable manual or large reference, keep it in the normal documentation
tree instead.
