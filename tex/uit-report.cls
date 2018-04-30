\NeedsTeXFormat{LaTeX2e}
\ProvidesClass{uit-report}[2014/12/10 UiT report class]
\def\baseclass{article}
\DeclareOption*{\PassOptionsToClass{\CurrentOption}{\baseclass}}
\PassOptionsToClass{12pt}{\baseclass}
\PassOptionsToClass{a4paper}{\baseclass}
\PassOptionsToClass{oneside}{\baseclass}
\PassOptionsToClass{titlepage}{\baseclass}
\PassOptionsToClass{pdftex}{\baseclass}
\ProcessOptions\relax
\LoadClass{\baseclass}
\special{papersize=210mm,297mm}

\pdfpageheight=297mm
\pdfpagewidth=210mm

\RequirePackage[utf8]{inputenc}
\RequirePackage[resetfonts]{cmap}
\RequirePackage[T1]{fontenc}
\RequirePackage{titlesec}
\RequirePackage{sectsty}
\RequirePackage{lmodern}
\RequirePackage{type1cm}
\RequirePackage{fixltx2e}

%\usepackage[a4paper, bottom=1.0in, left=1.1in, right=1.1in]{geometry}
\usepackage[a4paper, bottom=1.0in, outer=1.0in, inner=1.2in, headsep=140pt, twoside]{geometry}
\usepackage[usenames,dvipsnames]{color}
\usepackage[table]{xcolor}
\usepackage{soul}

\usepackage[obeyspaces,hyphens]{url}
\def\url@leostyle{%
  \@ifundefined{selectfont}{\def\UrlFont{\sf}}{\def\UrlFont{\small\ttfamily}}%
}
\urlstyle{leo}

\usepackage[pdftex,pagebackref=false,hyperindex,hyperfigures]{hyperref}
\usepackage[pdftex]{graphicx}
\usepackage[all]{hypcap}
\hypersetup{
    breaklinks=true,%
    colorlinks=false,%
    citecolor=black,%
    filecolor=black,%
    linkcolor=black,%
    urlcolor=black,%
    hidelinks%
}

\usepackage{float}
\usepackage[hang,bf,small,hypcap]{caption}
\usepackage{subcaption}
\usepackage{listings}

\let\ult@savetitle\title
\def\title#1{%
  \gdef\ult@title{#1}%
  \ult@savetitle{#1}%
}
\global\let\ult@author\@empty
\def\@author{\@latex@error{No \noexpand\author given}\@ehc}
\let\ult@saveauthor\author
\def\author#1{%
  \gdef\ult@author{#1}%
  \ult@saveauthor{#1}%
}
\def\subtitle#1{\gdef\@subtitle{#1}}

\usepackage{lastpage}
\usepackage{verbatim}
\usepackage[parfill]{parskip}
\usepackage{setspace}

\usepackage[nottoc]{tocbibind}
\usepackage[titles]{tocloft}
\usepackage{fancyhdr}

\fancypagestyle{firststyle}
{
	\fancyhf{}
	\renewcommand{\headrulewidth}{0pt}
	\fancyfoot[C]{\footnotesize \thepage\ }
}

\fancypagestyle{secondstyle}
{
	\fancyhf{} % sets both header and footer to nothing
	%\renewcommand{\headrulewidth}{0.75pt}
	
	
	
	%\lhead{\textcolor{darkgray}{\fancyplain{}{\ult@title}}}
	%\rhead{\textcolor{darkgray}{\fancyplain{}{June 2018}}}
	
	\lfoot{}
	%\cfoot{\textcolor{darkgray}{\fancyplain{}{Page {\thepage} of \pageref{LastPage}}}}
	\cfoot{\textcolor{darkgray}{\fancyplain{}{{\thepage}}}}
	\rfoot{}

}




\allsectionsfont{\rmfamily\bfseries}
\renewcommand{\cftsecfont}{\rmfamily\bfseries\upshape}
\renewcommand{\cftsecpagefont}{\rmfamily\bfseries\upshape}
\setlength{\headheight}{15pt}

\usepackage{amsfonts}
\usepackage{amsmath}
\usepackage{amsthm}
\usepackage{amssymb}
\usepackage{amscd}
\usepackage{xspace}
\usepackage{mathtools}
\usepackage{wasysym}
\usepackage{ifsym}
\usepackage{bm}
\usepackage[version=3]{mhchem}
\usepackage{upgreek}

\renewcommand{\baselinestretch}{1.5}

\endinput
