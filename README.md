# multithreaded-CLI-BubbleShooter
multithreaded bubble shooter using ncurses and ASCII art

**[Presentation Link](https://docs.google.com/presentation/d/1rnVmdZPNsP2O2WfdRQcZvhk5GMDKzemOTQnJTwN9wZM/edit?usp=sharing)**  

Members:
- (1908919) Ahmad Izzuddin
- (1908853) Arrozy Adi Falaqi
- (1901102) Aysha Alia Iskandar
- (1908589) Nenden Citra S.N
- (1905378) Zulfikar Ali Yunara P.

*Ini adalah repo game bubble shooter multithreaded di terminal yang menggunakan art ascii*

Copy the repo using:  
`gh repo clone nidduzzi/multithreaded-CLI-BubbleShooter`  
`git clone https://github.com/nidduzzi/multithreaded-CLI-BubbleShooter.git`  
Then move to the bin folder using:  
`cd multithreaded-CLI-BubbleShooter/src`  

Compile using:  
Debug       : `gcc -g -Wall main.c lib.c -o main -lpthread -lncursesw -ltinfo -lm`  
Run using   : `./main`  
Release     : `gcc -static main.c lib.c -o cli-bubbleshooter -lpthread -lncursesw -ltinfo -lm`  
Run using   : `./cli-bubbleshooter`  

**Self compiled binaries will need to follow the [file structure](#filestructure) refrenced in Releases**  

# Controls  
Left/Right Arrow keys to change bubble direction  
Enter to shoot  
Q to return to main menu  
Up/Down Arrow keys to select in menu  
Enter To confirm selection  

# Releases  
Releases are statically linked binaries compiled on Ubuntu 20.04 LTS  
<a id="filestructure"></a>File structure is as follows:

            ./
              bin/
                  executable binary (cli-bubbleshooter)
              data/
                   arrow.txt
                   bubble.txt
                   l1/
                      layout.txt
                   l2/
                      layout.txt
                   l3/
                      layout.txt
                   l4/
                      layout.txt
            
**Self compiled versions will also need to follow this structure**
