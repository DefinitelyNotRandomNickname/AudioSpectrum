# AudioSpectrum
### Description:
This project aims to provide a visually appealing representation of the audio spectrum from your audio input device.
It utilizes Fast Fourier Transformation (FFT) to analyze the audio data and presents it as either a 2D or 3D animation.

## Main application

### Main
The `main.cpp` file is responsible for creating the application and its main widget, and running it. It serves as the entry point for the program.

### Main Widget
The `mainwidget.cpp` and `mainwidget.h` files play a crucial role in this project. They are responsible for creating the necessary user interface (UI)
components for both the 2D and 3D visualizations. The main widget initializes the engine, establishes the connections between various functionalities,
and manages the different classes involved in the project. Depending on the selected scene, this class visualizes either the 2D or 3D scene on
the left side of the interface, while providing UI buttons and switches on the right side.

The main widget offers several features, including language switching between Polish and English, the ability to adjust the FPS (frames per second)
in the range of 1 to 120, the option to change the audio input device, the ability to set the color or gradient of the visualization,
and the capability to modify its characteristics.

## Audio input and FFT

### Engine
The `engine.cpp` and its accompanying header file `engine.h` are responsible for managing audio devices, selecting the appropriate data format,
and initiating the necessary calculations. These files allow you to configure and set up your desired audio device, enabling the sampling of
its audio. The engine probes the audio X times per second, depending on the frames per second (FPS) value that you specify.

### Spectrum Analyser
The `spectrumanalyser.cpp` and `spectrumanalyser.h` files retrieve the audio data from the engine and process it to obtain frequency, amplitude,
and phase information. This component utilizes a third-party implementation of the Fast Fourier Transformation algorithm to perform
the necessary calculations.

### Frequency Spectrum
The `frequencyspectrum.cpp` and `frequencyspectrum.h` files are responsible for storing the output of the FFT. The stored data is represented
as a list of elements, each containing frequency, amplitude, and phase information. This class allows us to manipulate the stored information
as if it were an array, thanks to the operator overloading functionality.

## 2D Scene

### UI
The user interface (UI) in the 2D scene provides several options to customize the visualization. It allows users to switch to the 3D scene,
adjust the FPS, change the input device, set the gradient for the visualization, and specify the number of bars displayed in the scene.
Currently, there are two available gradients to choose from, but you can easily add more by adding a series of hex values to the `gradient`
vector of strings in the `mainwidget.cpp` file. The number of bars can be set within the range of 32 to 1024, although the optimal
visualization range is between 64 and 256.

### Visualization
The visualization in the 2D scene is implemented through the `spectograph.cpp` and `spectograph.h` files, which are separate widgets
embedded within the mainwidget class.

The visualization process involves creating the specified number of bars and assigning them values based on the calculated spectrum using
a logarithmic distribution. These bars are then painted accordingly. However, due to the logarithmic distribution, some bars may have empty values.
To address this, the empty bars are filled with simulated values.

To improve the visual aesthetics, the values of the bars are smoothed based on neighboring bars and delayed based on previous values.
After painting the bars, the amplitudes of the low frequencies are measured, and if they exceed 5%, a variable responsible for creating a
glow effect is activated. The glow effect is achieved by drawing circles with a larger radius on the bars, and the transparency of the circles
is adjusted based on the aforementioned variable.

![](https://github.com/DefinitelyNotRandomNickname/AudioSpectrum/blob/main/images/2D_scene.gif)

## 3D Scene

### UI
The user interface (UI) in the 3D scene provides options to switch to the 2D scene, adjust the FPS, change the input device, and modify the color
of the sphere. Currently, there are five available colors for the sphere, but you can add more by including additional color names in the `color`
array of char pointers in the `mainwidget.cpp` file. To make the new color work, name it using an existing color name available in Qt, and add
a corresponding case in the switch statement within the `setColor` method in the `scene.cpp` file.

### Visualization
The visualization in the 3D scene is implemented through the `scene.cpp` and `scene.h` files, which are separate widgets embedded within the
`mainwidget` class. These files create a 3D entity where mini spheres are generated using the `sphere.cpp` and `sphere.h` files.

The visualization involves a camera positioned in the 3D scene, pointing towards the mini spheres. The number of mini spheres is determined
by the defined amount of horizontal and vertical halos in the `scene.h` file, specifically the variables `halos_2D` and `halos_3D`. The positions
of the spheres in 3D space are calculated using simple trigonometry, taking into account the central point of the scene, the radius of the circle (`radius`),
and a for loop to adjust the angles. The arrangement of the spheres in circles is influenced by the value of `halos_3D`, where higher halo values correspond
to lower frequencies, and lower halo values correspond to higher frequencies. The color of the mini spheres is white, but it reflects light, so by modifying
the light color, you can observe changes in the sphere's appearance.

The mini spheres are constructed as low-quality sphere meshes, consisting of three rings and three slices, with a fixed radius of 0.03.
They also store their initial coordinates for ease of calculations.

Similar to the spectograph, the 3D scene provides functionalities such as adjusting the FPS, changing the input device, and modifying the color of the sphere.
However, it does not include the missing bar filling or the glow effects.

![](https://github.com/DefinitelyNotRandomNickname/AudioSpectrum/blob/main/images/3D_scene.gif)

### Note
To run this program on windows you need QtCreator. If you are on Linux there is already built version in releases.
