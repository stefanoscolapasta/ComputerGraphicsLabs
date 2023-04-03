# Computer Graphics Labs
This repo serves me the sole purpose to track my work on the labs.

## LAB 1: SDL 
Getting to know SDL2.
<br/><br/>
![](./res/lab1_imgs/starfield.gif)
<br/><br/>
## LAB 2: RAYTRACER
Time to build a Raytracer with SDL2.
<br/><br/>
<h3>Initial attempts</h3>
<p align="left">
      <img src="./res/lab2_imgs/lab2Error_but_it_was_focal_length_lol.png" width="32%" >
      <img src="./res/lab2_imgs/lab2_understood_error.png" width="32%">
      <img src="./res/lab2_imgs/no_lighting.png" width="32%">
</p>
It took a while to understand it was not a mistake but simply the focal length was too small. Adjusting it made me realize what the problem was.
<br/><br/><br/><br/>
<h3>Added lighting</h3>
<p align="left">
      <img src="./res/lab2_imgs/first_illumination_attempt.png" width="45%" >
      <img src="./res/lab2_imgs/gif_illumination.gif" width="45%" >
</p>
Lighting intensity with no colors nor shadows, resolution is 100x100 to make interactivity possible.
<br/><br/><br/><br/>
<p align="left">
      <img src="./res/lab2_imgs/illumination_no_shadows.png" width="45%">
      <img src="./res/lab2_imgs/gif_illumination_and_colors.gif" width="45%">
</p>
Colors added in together with lighting.
<br/><br/><br/><br/>
<p align="left">
      <img src="./res/lab2_imgs/final.png" width="45%">
      <img src="./res/lab2_imgs/finalGif.gif" width="45%">
</p>
After adding in shadows + direct lighting + ambient lighting

## LAB 3: RASTERIZER
After building a simple raytracer, it's time to build a resterizer!
<br/><br/>
<p align="left">
      <img src="./res/lab3_imgs/finally_projection_working_it_was_integer_casting.png" width="45%">
      <img src="./res/lab3_imgs/colors_rasterized_no_depth_yet.png" width="45%">
</p>
First, projection was implemented, while also drawing lines between these projected points. Then color is added by drawing lines from one side to the other of each triangle (polygon). Z-buffer not yet implemented.

### Per vertex illumination
<p align="left">
      <img src="./res/lab3_imgs/correct_lighting_per_vertex.png" width="45%">

</p>
Lighting is calculated per vertex, and then values are interpolated along the polygon. Z-buffer was also implemented to account for depth.

### Per pixel illumination

<p align="left">
       <img src="./res/lab3_imgs/per_pixel_wrong_pos_2d_interp.png" width="45%">
       <img src="./res/lab3_imgs/per_pixel_right_pos_2d_interp.png" width="45%">
</p>
Maybe not immediately noticeable, but looking closely, lighting effect is a bit skewed.
<br/>
The second image does not have the skew effect because perspective correct
interpolation was used for the 3D position of the pixels.
<br/><br/><br/><br/>
<p align="left">
       <img src="./res/lab3_imgs/per_pixel_right_pos_2d_interp.png" width="45%">
</p>
In this case, no interpolation is performed for the lighting, but pixel positions are interpolated. Lighting is then calculated for each pixel, making the final result more accurate.

## Camera Movement + Light Source Movement
<p align="left">
      <img src="./res/lab3_imgs/gif_per_vertex_illumination.gif" width="45%">
       <img src="./res/lab3_imgs/final_gif_wrong_3dpos_interpolation.gif" width="45%">
</p>
