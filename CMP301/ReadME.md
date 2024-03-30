# Graphics Project, DirectX11 with Shaders. Doing Tessellation, Lighting, Shadows, Vertex Manipulation, and Post-Processing

## Information

[Application EXE](https://drive.google.com/drive/folders/1RMM2ecpw69IVUgEdB33Sr3fYxV5SsCX8?usp=sharing)
[TanapatSomrid_2000891_Report.pdf](https://github.com/TaniCorn/Abertay-Projects/files/14812101/TanapatSomrid_2000891_Report.pdf)
This project was written for my CMP301 module in 3rd year Abertay 2022-2023, where I recieved an A+.

What can you expect from this page?

- [Initial project design and thoughts](#project-design)
- [Feedback recieved and addressed](#individual-feedback-recieved)
- [Retrospective](#retrospect)


What can you expect from the code?

- Harder Concepts like Tessellation, vertex manipulation, normal recalculation, and post-processing implemented
- Lighting and shadow which follows the Frank Luna book
- Most of the code written in hlsl

What can you expect from the program?

- A small scene that has good IMGUI user interface to display all techniques with varying degrees of control
![overview](https://user-images.githubusercontent.com/63819551/236672828-1e9fb187-b2c1-4796-883d-402ee9f1ffbe.png)

## Project supporting document
### Project Design

When I first started this project I had a couple goals:
1. Use camera, and steepness dynamic tessellation on the vertex manipulated terrain
2. Have a lighting system that could switch to different lighting models
3. Complete the standard kernal post-processing system and develop a distinctive post-processing
4. Just get shadows to work

I tried to avoid shadows because it was simply not on my radar for things to experiment with.
I did a lot of research in books like Frank Luna DX11 and Practical Rendering and Computation which is where I recieved the inspiration from for my own tessellation shader.

### Devlog

Lighting
 - Lighting was pretty simple and was just more of a fun thing to implement when I had time
 - Coming up with the user interface with ImGui was more harder, I think my UX improved :)

Vertex Manipulation
 - Initially I thought my vertex manipulated normals were correct, however after verifying it by only rendering one normal at a time, I realise that it was slightly wrong.
 - Had to move the normal recalculation to the domain shader for more accurate tessellated calculations

Post-processing
 - Implemented a simple 3x3 kernal shader system that could use different kernal post-processes
 - Researched a lot of maths and implemented my own variable size kernal shader, specifically for gaussian blurring.

Tessellation
- Struggled through the pain that was understanding tessellation
- Started debugging how the tessellator worked through Renderdoc
- Started drawing out how the tesselator worked
- Tessellated squares, based on two triangles, and their midpoints.

# Post-SFAS

Unfortunately I've not had much of a chance to go back and clean this up, as I've moved onto different concepts. However I'm still highly interested in rendering and would love the chance to tackle Vulkan or DX12.

## Retrospect

- This was a really challenging project trying to make sure my things worked, as I had a lot of advanced concepts to do in a matter of months. However I'm really proud that I managed to do it.

- I realise that the code is not up to scratch with many standards. I want to go back someday to clean this up.

![Tess](https://user-images.githubusercontent.com/63819551/236672836-dbe3fa31-ca16-41c4-bf04-5592a85f1a8a.png)
![Screenshot 2022-10-20 113035](https://user-images.githubusercontent.com/63819551/236672839-1f2a5a47-1fee-4c5d-9706-589f65035d56.png)
![post-processing](https://user-images.githubusercontent.com/63819551/236672846-12948a63-d86e-49c8-9953-816102a36843.png)
![CMP303 Lighting](https://user-images.githubusercontent.com/63819551/236673041-f97fae68-c730-43ef-9227-a5097312aa7c.png)
