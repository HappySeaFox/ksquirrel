<h1 align="center">KSquirrel</h1>
<h3 align="center">Extremely fast image viewer for Linux with KDE3.</h3>

<p align="center">
  <a href=".github/screenshot.png"><img src=".github/screenshot.png?raw=true" alt="KSquirrel Screenshot"/></a>
</p>

:warning: KSquirrel is not maintained anymore. It doesn't support modern Linux distros and KDE >= 4. Even though
simplicity and modularity lay in the heart of KSquirrel, some of its design decisions or implemented classes
are really controversial. Specifically, singletons are heavily used, which leads to highly coupled classes
with unnecessary dependencies.

The image codecs from KSquirrel have been rebranded in 2020 as a fast and lightweight cross-platform image library called [SAIL](https://github.com/HappySeaFox/sail).
You can still try KSquirrel in the [Trinity Desktop](https://www.trinitydesktop.org).
