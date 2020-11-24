# multithreaded-CLI-BubbleShooter
multithreaded bubble shooter using ncurses and ASCII art

*Ini adalah repo game bubble shooter multithreaded di terminal yang menggunakan art ascii*

copy the repo using:  
`gh repo clone nidduzzi/multithreaded-CLI-BubbleShooter`  
then move to the bin folder using:  
`cd multithreaded-CLI-BubbleShooter/bin`  
compile using:  
`gcc *.c -Wall -g -o main -lncurses`  
run using:  
`./main`  

# Game process

MENU
1. play
2. select level
3. quit


-----------------
|r b r r r b b r|
|b b b b b b b  |
| B   r         |
|               |
|               |
|               |
    angle = 0

## TODO: GAME_INIT

`g_o_t game = {...,load(bullet.txt), ...}`

bullet.txt



        /``\
        \../
        ####
        rrrr
        rwrr
        ####
        rrrr
        rrrr


## TODO: BULLET_READY

KEY_A, KEY_D --> `angle_rad += (+/-)0.01`

straight line function: `y=mx+c` 

SPACE --> change state(BULLET_FIRED)

## TODO: BULLET_FIRED

double v = 0.05 /sec

time() - old_time > 0.5
posx += sin(angle)*v
x = b1x - posx
y = b1y -posy
r = sqrt(x^2 + y^2)
if(r<v) -->change state(BULLET_HIT)
old_time = time()

## TODO: BULLET_HIT

score = count(number of bubbles popped)
if(masih ada bubble) change state(BULLET_FIRED)
else change state(GAME_END)
