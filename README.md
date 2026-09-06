# Financial_News_Bot

## Purpose

I have wanted to start developing a way to pull stock market information and do analysis on it. Additionally, I have not had a chance to do much building using AI and agnets to help generate code. As such this project was initially created with OpenCode and the Granite4.1:3b model running in Ollama.

## Building

These are the steps and commands that I used on my machine to build this project:

```
```powershell
mkdir build
cmake .. -DBUILD_SHARED_LIBS=OFF -DCPR_USE_SYSTEM_CURL=OFF
cmake --build . --parallel
```
```
