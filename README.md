# window-timer
Final Project for CS 193. Personal tool that tracks time spent on a supported browser + minecraft.  

Built for my personal use.  

NOTE: only compatible with windows  

Supported browsers:  
Brave  
Chrome  
Firefox  

# The Problem

Built for my multitasking problems. Playing Minecraft and doing homework at the same time can lead to getting carried away with Minecraft. This project aims to keep accountability by keeping a constant visual of my time split between Minecraft and my browser (homework).  

# How It Works

Detects the foreground process and matches it with defined exe names to track time spent on either "Homework" or "Minecraft."  

![System Diagram](./System%20Diagram.png)  

# Tech Stack

Uses C++ with the Win32 API (windows.h)  

# Setup

Clone the repo (git clone https://github.com/jbz4/window-timer)  

Make sure you have your browser open (Chrome, Firefox, or Brave) and Minecraft open  

Run the main.exe file in window-timer/scripts/  

# MVP Scope

- [x] Create an overlay

- [x] Show times for 2 windows

- [x] Tracking support for Minecraft + browser(s)

# Future Plans

- [ ] Create support for multiple windows/processes  

- [x] Idle detection  

- [ ] Add notifications  

# DEMO

TODO: put video link here