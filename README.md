# CS2015701 Project Showcase

> [!Important]
> This repository might be irrelevant to the future course after **Spring Semester 2026**, as the professor has announced updates to the syllabus and project requirements.
> You can see the requirements of Spring Semester 2026 and check if it is the same as yours.
> 
> 教授說明年開始(現在是114第二學期)因應產業的轉變(AI)會改專案要求，作業大綱可能會與此Repo展現的不一樣，這課可以vibe coding，重點是要知道架構跟原理。

## Features

* **Project 1:** 
A **robot animation editor**,implement through Node-based Hierarchical Transformation, need to do some required movement, such as APT dance.
**動作編輯器**，靠階層式變換實作(不能用骨架)，有指定的動作例如APT的舞。
* **Project 2:** 
An extension of Project 1, a **simple visual effect editor** to implement some effect such as firework that is synchronized with the robot's dance choreography.
Project1的延伸，一個簡單的**特效編輯器**，要實作一些指定的特效，例如讓煙火在機器人舞蹈的節奏上綻放。
* **Project 3:** 
An **texture editor** for specify model.
**材質編輯器**，要透過拆UV去讓texture好好貼合模型。

> [!NOTE]
> Project 3 will be updated later,still working on remaking it.
> 應該會重製Project 3，之後才會放上來。

## Environment
* **OS:** Linux (CachyOS w/ hyprland)/ Windows 11
* **Language:** C++ / GLSL
* **Tools/Libraries on Linux:** CMake, GCC / Clang, 
System libraries (`opengl-headers`, `glew`, `glfw-x11`, `libx11`),
Bundled libraries (`ImGui`, `GLM`,`nlohmann/json`, `stb_image`).
* **Tools/Libraries on Windows:** 
CMake, MSVC (Visual Studio Build Tools with `/MT` flag), 
Pre-compiled static libraries (`glew32s.lib`, `glfw3_mt.lib`), 
Windows SDK (`opengl32`, `gdi32`, `user32`, `shell32`),
Bundled libraries (`ImGui`, `GLM`,`nlohmann/json`, `stb_image`).

> [!NOTE]
> The discription of tools and libraries is for project 1 & 2.
> 這些工具還有套件是Project 1 & 2用到的。

## Build & Run
**For bulid :**
* on Linux, in a project folder:
```
mkdir build && cd build
cmake ..
make
```
* on windows, in a project folder:
```

```

**For Run :**
make sure your executable is in the same directory with `res/` folder

## Repository Structure
待補
## Implementation Note
待補
## Academic Integrity & Disclaimer

This repository is dedicated solely to showcasing personal project achievements. 
* **No Plagiarism:** Future students enrolled in this or related courses are strictly prohibited from copying, modifying, or redistributing any code or documentation from this repository for graded assignments.
* **Academic Integrity:** Utilizing this code in your course submissions without proper attribution constitutes plagiarism and violates academic integrity regulations.
* **Accountability:** The author holds no responsibility for any academic penalties, failing grades, or disciplinary actions incurred by others due to the unauthorized or inappropriate use of this codebase.
