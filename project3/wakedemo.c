#include <msp430.h>
#include <libTimer.h>
#include "lcdutils.h"
#include "lcddraw.h"
#include "buzzer.h"
#include "song.h"


//All Bits that I'll be using for this lab (Switches and LED)
#define LED BIT6
#define SW1 1
#define SW2 2
#define SW3 4
#define SW4 8

#define SWITCHES 15

//Here I have all the colors for the squares, the number of colors, and the current one
unsigned short s_colors[] = {COLOR_DARK_VIOLE, COLOR_DARK_GREEN, COLOR_RED};
int curr_color = 0;
int num_colors = 3;

//Here I have all the positions where the squares can appear on the screen, number of positions and current position
unsigned short position_row[] = {30, 80, (screenHeight-20), 120, 145, 44, 30};
unsigned short position_col[] = {10, (screenWidth-20), 80, 30, 100, 89, 78};
int position = 0;
int num_positions = 7;

/* Update switch interrupt to detect changes from current buttons */
static char 
switch_update_interrupt_sense()
{
  char p2val = P2IN;
  
  P2IES |= (p2val & SWITCHES);	/* if switch up, sense down */
  P2IES &= (p2val | ~SWITCHES);	/* if switch down, sense up */
  return p2val;
}

/* Set up switches */
void 
switch_init()			
{  
  P2REN |= SWITCHES;		/* enables resistors for switches */
  P2IE |= SWITCHES;		/* enable interrupts from switches */
  P2OUT |= SWITCHES;		/* pull-ups for switches */
  P2DIR &= ~SWITCHES;		/* set switches' bits for input */
  switch_update_interrupt_sense();
}

int switches = 0;

/* Checks if an interrupt in the switches happens */
void
switch_interrupt_handler()
{
  char p2val = switch_update_interrupt_sense();
  switches = ~p2val & SWITCHES;
}

short redrawScreen = 1;
void song();
int count = 0;
int second = 0;


void wdt_c_handler()
{
  static int secCount = 0;
  
  secCount ++;
  if (secCount >= 25) {		/* 10/sec */
    secCount = 0;
    redrawScreen = 1;
  }
  count++;
  if (count >= 250){
    count = 0;
    second++;
    song();
  }
}

/* Functions that I'll use to show the Rules and to update screen */
void update_shape();
void rules();
void update_score();

int incorrect = 0;
char correct[]= {'0', '0', '0'};
int ruling = 0;

void main()
{
  
  P1DIR |= LED;		/**< Green led on when CPU on */
  P1OUT |= LED;
  configureClocks();
  lcd_init();
  switch_init();
  buzzer_init();
  
  enableWDTInterrupts();      /**< enable periodic interrupt */
  or_sr(0x8);	              /**< GIE (enable interrupts) */
  
  clearScreen(COLOR_BLACK);
  while(ruling<=250){        /* Show rules on the screen */
    rules();
  }
 
  clearScreen(COLOR_WHITE);
  
  while (incorrect < 5) {			/* Keep playing until user gets 5 wrong */
    if (redrawScreen) {
      redrawScreen = 0;
      update_shape();
    }
    P1OUT &= ~LED;	/* led off */
    or_sr(0x10);	/**< CPU OFF */
    P1OUT |= LED;	/* led on */
  }
  clearScreen(COLOR_RED);
  drawString5x7(40, 70, "GAME OVER", COLOR_WHITE, COLOR_BLACK);
  drawString5x7(82, 80, correct, COLOR_WHITE, COLOR_BLACK);
  drawString5x7(40, 80, "SCORE: ", COLOR_WHITE, COLOR_BLACK);
  

}

/* Rules are just a lot of drawStrings calls */
void
rules()
{
  if (ruling < 125){
    drawString5x7(60, 20, "S^2", COLOR_WHITE, COLOR_RED);
    drawString5x7(10, 35, "In this game you ", COLOR_WHITE, COLOR_RED);
    drawString5x7(10, 50, "have to guess the ", COLOR_WHITE, COLOR_RED);
    drawString5x7(10, 65, "color of the square ", COLOR_WHITE, COLOR_RED);
    drawString5x7(10, 80, "by pressing the ", COLOR_WHITE, COLOR_RED);
    drawString5x7(10, 95, "correct switch.", COLOR_WHITE, COLOR_RED);
    drawString5x7(10, 110, "SW1 is PURPLE.", COLOR_WHITE, COLOR_RED);
    drawString5x7(10, 125, "SW3 is GREEN.", COLOR_WHITE, COLOR_RED);
    drawString5x7(10, 140, "SW4 is RED.", COLOR_WHITE, COLOR_RED);
  }
  else if (ruling == 125){
    clearScreen(COLOR_BLACK);
  }
  else{
    drawString5x7(8, 35, "Every 10 correct ", COLOR_WHITE, COLOR_RED);
    drawString5x7(8, 50, "guesses will ", COLOR_WHITE, COLOR_RED);
    drawString5x7(8, 65, "increase the speed.", COLOR_WHITE, COLOR_RED);
    drawString5x7(8, 80, "Five incorrect ", COLOR_WHITE, COLOR_RED);
    drawString5x7(8, 95, "guesses and you lose.", COLOR_WHITE, COLOR_RED);
  }
  ruling++;
}

/* Updates the score, since I used an array of chars, I have to reset score if it goes above 9 */
void
update_score()
{
  if (correct[2] == '9'){
    correct[2] = '0';
    if (correct[1] == '9'){
      correct[1] = '0';
      if (correct[0] == '9'){
	correct[0] = '0';
      }
      else{
	correct[0]++;
      }
    }
    else{
      correct[1]++;
    }
  }
  else{
    correct[2]++;
  }
  
}

/* Variables I'll use in update screen*/
int step = 0;
int guess = 0;
int squares = 0;
int speed = 60;
int srand();

/* Updates screen, prints put a square at a random position and random color */
void
update_shape()
{
 
  fillRectangle(0, 0, screenWidth, 15, COLOR_BLACK);
  drawString5x7(45, 5, "SCORE: ", COLOR_WHITE, COLOR_RED);
  drawString5x7(80, 5, correct, COLOR_WHITE, COLOR_RED);
  
  if (step <= speed) {
   
    int width = 20;
    int height = 20;

    fillRectangle(position_col[position], position_row[position], width, height, s_colors[curr_color]);
    squares++;
    if (switches & SW4){
      if (curr_color == 2){
	update_score();
	guess++;
      }
      else{
	incorrect++;
      }
      step = speed;
    }
    else if (switches & SW3) {
      if (curr_color == 1){
	update_score();
	guess++;
      }
      else{
	incorrect++;
      }
      step = speed;
    }
    else if (switches & SW1) {
      if (curr_color == 0){
	update_score();
	guess++;
      }
      else{
	incorrect++;
      }
      step = speed;
    }
    step ++;
    
  } else {
    clearScreen(COLOR_WHITE);
    step = 0;
    int rand();
    curr_color = rand() % num_colors;
    position = (position+1) % num_positions;
    if (guess%10 == 0 && speed > 10){
      speed = speed - 10;
    }
  }
}


/* Is the song for the game. This function was translated to Assembly */

/*
void
song()
{
  switch(second%6){
  case 1:
    buzzer_set_period(1516);
    break;
  case 2:
    buzzer_set_period(1911);
    break;
  case 3:
    buzzer_set_period(1275);
    break;
  case 4:
    buzzer_set_period(2551);
    break;
  case 5:
    buzzer_set_period(3033);
    break;
  default:
    buzzer_set_period(2272);
    break;
  }
}*/

/* Switch on S2 */
void
__interrupt_vec(PORT2_VECTOR) Port_2(){
  if (P2IFG & SWITCHES) {	      /* did a button cause this interrupt? */
    P2IFG &= ~SWITCHES;		      /* clear pending sw interrupts */
    switch_interrupt_handler();	/* single handler for all switches */
  }
}
