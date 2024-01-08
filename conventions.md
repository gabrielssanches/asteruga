## C Coding Style Conventions

Code element | Convention | Example
--- | :---: | ---
Defines | ALL_CAPS | `#define PLATFORM_DESKTOP`
Macros | ALL_CAPS | `#define MIN(a,b) (((a)<(b))?(a):(b))`
Variables | snake_case | `int screenWidth = 0;`, `float target_frame_time = 0.016f;`
Local variables | snake_case | `Vector2 player_position = { 0 };`
Global variables | snake_case | `bool game_quit = false;`
Constants | snake_case | `const int max_value = 8;`
Pointers | MyType *pointer | `Texture2D *array = NULL;`
float values | always x.xf | `float gravity = 10.0f`
Operators | value1*value2 | `int product = value*6;`
Operators | value1/value2 | `int division = value/4;`
Operators | value1 +value2 | `int sum = value +10;`
Operators | value1 -value2 | `int res = value -5;`
Enum | snake_case | `enum texture_format`
Enum members | ALL_CAPS | `PIXELFORMAT_UNCOMPRESSED_R8G8B8`
Struct | snake_case | `struct texture_2d`, `struct material`
Struct typedef | snake_case | `typedef struct texture { ... } texture;`
Struct members | snake_case | `texture.width`, `color.r`
Functions | snake_case | `window_init()`, `image_load_from_memory()`
Functions params | snake_case | `width`, `height`
Ternary Operator | (condition)? result1 : result2 | `printf("Value is 0: %s", (value == 0)? "yes" : "no");`

Other conventions:
 - Four spaces are used, instead of TABS
 - Trailing spaces are always avoided
 - Control flow statements are always guarded with {} and followed **by a space**:
```c
if (condition) {
   // do something
}

while (!WindowShouldClose()) {
   // do something
}

for (int i = 0; i < NUM_VALUES; i++) {
    printf("%i", i);
}

switch (value) {
    case 0: {
        // do something
        break;
    }
    case 2: {
        // do another thing
        break;
    }
    default: {
        break;
    }
}
```
 - All conditions are always between parenthesis, but not boolean values:
```c
if ((value > 1) && (value < 50) && valueActive) {
   // do something
}
```
 - Braces and curly brackets always open in the same line:
```c
void some_function() {
   // do something
}
```

## Files and Directories Naming Conventions

  - Directories are named using `snake_case`: `resources/models`, `resources/fonts`
  - Files are named using `snake_case`: `main_title.png`, `cubicmap.png`, `sound.wav`

_NOTE: Spaces and special characters are always avoided in the files/dir naming!_

## Games/Examples Directories Organization Conventions

 - Resource files are organized by context and usage in the game. Loading requirements for data are also considered (grouping data when required).
 - Descriptive names are used for the files, just reading the name of the file it should be possible to know what is that file and where fits in the game.

```
resources/audio/fx/long_jump.wav
resources/audio/music/main_theme.ogg
resources/screens/logo/logo.png
resources/screens/title/title.png
resources/screens/gameplay/background.png
resources/characters/player.png
resources/characters/enemy_slime.png
resources/common/font_arial.ttf
resources/common/gui.png
```
_NOTE: Some resources require to be loaded all at once while other require to be loaded only at initialization (gui, font)._
