-- nova-filter.lua
-- Pandoc Lua filter for NovaBASIC user guide
-- Maps fenced divs to LaTeX tcolorbox environments
-- Maps basic code blocks to NovaBASIC listings

function Div(el)
  if el.classes:includes("note") then
    return pandoc.RawBlock("latex",
      "\\begin{notebox}\n" ..
      pandoc.write(pandoc.Pandoc(el.content), "latex") ..
      "\n\\end{notebox}")
  elseif el.classes:includes("warning") then
    return pandoc.RawBlock("latex",
      "\\begin{warningbox}\n" ..
      pandoc.write(pandoc.Pandoc(el.content), "latex") ..
      "\n\\end{warningbox}")
  elseif el.classes:includes("tip") then
    return pandoc.RawBlock("latex",
      "\\begin{tipbox}\n" ..
      pandoc.write(pandoc.Pandoc(el.content), "latex") ..
      "\n\\end{tipbox}")
  elseif el.classes:includes("tryit") then
    return pandoc.RawBlock("latex",
      "\\begin{tryitbox}\n" ..
      pandoc.write(pandoc.Pandoc(el.content), "latex") ..
      "\n\\end{tryitbox}")
  end
end

function CodeBlock(el)
  if el.classes:includes("basic") then
    return pandoc.RawBlock("latex",
      "\\begin{lstlisting}[language=NovaBASIC]\n" ..
      el.text ..
      "\n\\end{lstlisting}")
  end
end
