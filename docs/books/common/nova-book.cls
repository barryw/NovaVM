\NeedsTeXFormat{LaTeX2e}
\ProvidesClass{nova-book}[2026/05/22 Shared Nova book style]

\LoadClass[11pt,a4paper]{book}

\RequirePackage[utf8]{inputenc}
\RequirePackage[T1]{fontenc}
\RequirePackage{lmodern}
\RequirePackage{microtype}
\RequirePackage[a4paper, top=2.35cm, bottom=2.45cm, left=2.65cm, right=2.35cm,
                headheight=24pt]{geometry}
\RequirePackage{xcolor}
\RequirePackage{graphicx}
\RequirePackage{tikz}
\usetikzlibrary{calc,positioning}
\RequirePackage{array}
\RequirePackage{booktabs}
\RequirePackage{longtable}
\RequirePackage{tabularx}
\RequirePackage{listings}
\RequirePackage[skins,breakable]{tcolorbox}
\RequirePackage{fancyhdr}
\RequirePackage{titlesec}
\RequirePackage{titletoc}
\RequirePackage{hyperref}
\RequirePackage{svg}

\definecolor{novapaper}{RGB}{251,248,239}
\definecolor{novaink}{RGB}{27,27,27}
\definecolor{novablue}{RGB}{27,64,151}
\definecolor{novared}{RGB}{176,42,47}
\definecolor{novagold}{RGB}{224,172,47}
\definecolor{novagreen}{RGB}{38,134,84}
\definecolor{novacyan}{RGB}{31,154,188}
\definecolor{novapale}{RGB}{239,243,249}
\definecolor{novasoftgray}{RGB}{102,104,112}
\definecolor{novarule}{RGB}{188,201,226}
\definecolor{novascreen}{RGB}{244,247,252}

\pagecolor{novapaper}
\color{novaink}

\hypersetup{
  colorlinks=true,
  linkcolor=novablue,
  urlcolor=novablue,
  citecolor=novablue,
  bookmarks=true
}

\pagestyle{fancy}
\fancyhf{}
\fancyhead[LE]{\small\ttfamily\color{novablue}\thepage\quad\leftmark}
\fancyhead[RO]{\small\ttfamily\color{novablue}\rightmark\quad\thepage}
\fancyfoot[C]{}
\renewcommand{\headrulewidth}{0pt}
\renewcommand{\footrulewidth}{0pt}
\renewcommand{\headrule}{%
  \hbox to\headwidth{\color{novarule}\leaders\hrule height 0.8pt\hfill}}

\titleformat{\chapter}[display]
  {\normalfont\bfseries\color{novablue}}
  {\ttfamily\Large CHAPTER \thechapter}
  {0.45ex}
  {\titlerule[1.2pt]\vspace{0.8ex}\Huge}
  [\vspace{0.5ex}{\color{novacyan}\titlerule[0.8pt]}]
\titlespacing*{\chapter}{0pt}{0pt}{22pt}

\titleformat{\section}
  {\normalfont\Large\bfseries\color{novablue}}
  {\ttfamily\thesection}{0.65em}{}
  [\vspace{-0.35ex}{\color{novarule}\titlerule}]

\titleformat{\subsection}
  {\normalfont\large\bfseries\color{novaink}}
  {\ttfamily\color{novablue}\thesubsection}{0.6em}{}

\lstdefinelanguage{ca65asm}{%
  morekeywords=[1]{.include,.segment,.export,.import,.byte,.word,.res,.if,.ifdef,
    .ifndef,.endif,.else,.repeat,.endrepeat,.macro,.endmacro},
  morekeywords=[2]{LDA,LDX,LDY,STA,STX,STY,JSR,JMP,RTS,BRA,BEQ,BNE,BCC,BCS,
    BPL,BMI,CLC,SEC,CLI,SEI,STZ,INC,DEC,INX,INY,DEX,DEY,TAX,TAY,TXA,TYA,PHX,
    PLX,PHA,PLA,CMP,CPX,CPY,AND,ORA,EOR,ADC,SBC,ASL,LSR,ROL,ROR},
  morecomment=[l]{;},
  sensitive=true,
  morestring=[b]"%
}

\lstdefinestyle{novacode}{%
  language=ca65asm,
  basicstyle=\ttfamily\small\color{novaink},
  backgroundcolor=\color{novascreen},
  frame=tb,
  framerule=0.8pt,
  rulecolor=\color{novablue},
  framesep=6pt,
  xleftmargin=6pt,
  xrightmargin=6pt,
  keywordstyle=[1]\bfseries\color{novablue},
  keywordstyle=[2]\bfseries\color{novacyan!70!black},
  commentstyle=\color{novasoftgray}\itshape,
  stringstyle=\color{novagreen!70!black},
  showstringspaces=false,
  tabsize=2,
  breaklines=true,
  columns=fullflexible,
  keepspaces=true,
  numbers=none,
  aboveskip=0.9em,
  belowskip=0.9em,
  captionpos=b%
}
\lstset{style=novacode}

\tcbset{novaboxbase/.style={%
  enhanced,
  sharp corners,
  boxrule=0.8pt,
  left=8pt,
  right=8pt,
  top=6pt,
  bottom=6pt,
  colback=novapaper,
  fonttitle=\bfseries\ttfamily,
  coltitle=white,
  attach boxed title to top left={xshift=7pt,yshift=-2pt},
  boxed title style={sharp corners, boxrule=0pt, left=5pt, right=5pt, top=2pt, bottom=2pt},
  before upper={\raggedright},
  breakable%
}}

\newtcolorbox{novanote}[1][]{%
  novaboxbase,
  colframe=novablue,
  colbacktitle=novablue,
  title=Nova Note,
  #1%
}

\newtcolorbox{novawarning}[1][]{%
  novaboxbase,
  colframe=novared!85!black,
  colbacktitle=novared!85!black,
  title=Warning,
  #1%
}

\newtcolorbox{novaexample}[1][]{%
  novaboxbase,
  colframe=novagreen!80!black,
  colbacktitle=novagreen!80!black,
  title=Example,
  #1%
}

\newtcolorbox{ndkapibox}[3][]{%
  novaboxbase,
  colframe=novablue,
  colback=novapale!65!novapaper,
  colbacktitle=novablue,
  title={#2\hspace{0.6em}\textnormal{\small (#3)}},
  #1%
}

\newcommand{\novacode}[1]{\texttt{#1}}
\newcommand{\novasvg}[2][]{\includesvg[inkscapelatex=false,#1]{#2}}
\newcommand{\novabooktitle}[1]{\def\NovaBookTitle{#1}}
\newcommand{\novabooksubtitle}[1]{\def\NovaBookSubtitle{#1}}
\newcommand{\novabookversion}[1]{\def\NovaBookVersion{#1}}
\newcommand{\novabookdate}[1]{\def\NovaBookDate{#1}}
\newcommand{\NovaBookTitle}{Nova Book}
\newcommand{\NovaBookSubtitle}{}
\newcommand{\NovaBookVersion}{Draft}
\newcommand{\NovaBookDate}{}

\newcommand{\novamakecover}{%
  \begin{titlepage}
    \pagecolor{novapaper}
    \begin{tikzpicture}[remember picture,overlay]
      \fill[novablue] (current page.north west) rectangle ([yshift=-3.3cm]current page.north east);
      \fill[novacyan] ([yshift=-3.3cm]current page.north west) rectangle ([yshift=-3.55cm]current page.north east);
      \fill[novagold] ([xshift=1.6cm,yshift=-5.2cm]current page.north west) rectangle ([xshift=2.2cm,yshift=-23.5cm]current page.north west);
    \end{tikzpicture}
    \vspace*{1.1cm}
    {\color{white}\ttfamily\Large Nova Documentation Library\par}
    \vspace{3.2cm}
    {\color{novablue}\Huge\bfseries \NovaBookTitle\par}
    \vspace{0.45cm}
    {\color{novaink}\Large \NovaBookSubtitle\par}
    \vfill
    {\ttfamily\large \NovaBookVersion\par}
    \vspace{0.3cm}
    {\ttfamily\large \NovaBookDate\par}
  \end{titlepage}
}
