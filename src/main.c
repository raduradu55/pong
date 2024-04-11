
#include <stdio.h>
#include "include/SDL.h"
#include "constants.h"

// TODO: ball particles
// TODO: change ball color depending of the pad that bounced it
// TODO: add points when scored
// TODO: add interval between scores
// TODO: add start and pause match 

struct ball {
    float x;
    float y;
    float width;
    float height;
    int x_velocity;
    int y_velocity;
    int bounce_turn;
    int next_wall_bounce;
};

struct pad {
    float x;
    float y;
    float width;
    float height;
    int direction;
    int velocity;
    int points;
}; 

struct state {
    SDL_Window* window;
    SDL_Renderer* renderer;
    int running;

    struct time {
        float delta;
        float last_frame;
        float last_second;
        int frames;
    } time;

    struct pad pads[2];
    struct ball ball;

} state;

void resetBall(int restartDireciton) {
    state.ball.x = (WINDOW_WIDTH / 2) - (BALL_WIDTH / 2);
    state.ball.y = (WINDOW_HEIGHT / 2) - (BALL_HEIGHT / 2);
    state.ball.y_velocity = 0;

    if(restartDireciton == LEFT) {
        state.ball.x_velocity = -BALLVX_START;
        state.ball.bounce_turn = LEFT_PAD;
    }
    if(restartDireciton == RIGHT) {
        state.ball.x_velocity = +BALLVX_START;
        state.ball.bounce_turn = RIGHT_PAD;
    } 
}

void resetPads() {
    for(int padn = 0; padn<PADS; padn++) {
        state.pads[padn].y = (WINDOW_HEIGHT / 2) - (PAD_HEIGHT / 2);
    }
}

void bounceBall() {

    // check collision with left side pad
    if(state.ball.x < state.pads[LEFT_PAD].x + state.pads[LEFT_PAD].width &&
        state.ball.x + state.ball.width > state.pads[LEFT_PAD].x &&
        state.ball.y + state.ball.height > state.pads[LEFT_PAD].y &&
        state.ball.y < state.pads[LEFT_PAD].y + state.pads[LEFT_PAD].height &&
        state.ball.bounce_turn != RIGHT_PAD) {
        
        state.ball.bounce_turn = RIGHT_PAD;
        state.ball.next_wall_bounce = -1;

        // bottom and top side bounce
        if((state.ball.y > state.pads[LEFT_PAD].y + state.pads[LEFT_PAD].height - 1 && state.ball.y_velocity < 0 &&
            state.pads[LEFT_PAD].direction != DOWN) ||
            (state.ball.y + state.ball.height < state.pads[LEFT_PAD].y + 1 && state.ball.y_velocity > 0 &&
            state.pads[LEFT_PAD].direction != UP)) {
            state.ball.y_velocity = -state.ball.y_velocity;
        }
        // bottom and top side forword bounce
        else if((state.ball.y > state.pads[LEFT_PAD].y + state.pads[LEFT_PAD].height - 1 ||
            state.ball.y + state.ball.height < state.pads[LEFT_PAD].y + 1) &&
            (state.pads[LEFT_PAD].direction == DOWN || state.pads[LEFT_PAD].direction == UP)) {
            
            state.ball.x_velocity = BALLVX_TBSIDE;
            switch(state.pads[LEFT_PAD].direction) {
                case UP:
                    state.ball.y_velocity = -BALLVY_TBSIDE;
                    break;
                case DOWN:
                    state.ball.y_velocity = BALLVY_TBSIDE;
                    break;
            }
        }
        // bottom and top quarter bounce
        else if((state.pads[LEFT_PAD].direction == DOWN || state.pads[LEFT_PAD].direction == UP) &&
            (state.ball.y > (state.pads[LEFT_PAD].y + state.pads[LEFT_PAD].height) - (state.pads[LEFT_PAD].height / 4) ||
            state.ball.y + state.ball.height < state.pads[LEFT_PAD].y + (state.pads[LEFT_PAD].height / 4))) {
            
            state.ball.x_velocity = BALLVX_LOWQT;
            switch(state.pads[LEFT_PAD].direction) {
                case UP:
                    state.ball.y_velocity = -BALLVY_LOWQT;
                    break;
                case DOWN:
                    state.ball.y_velocity = BALLVY_LOWQT;
                    break;
            } 
        }
        // top and bottom middle quarter bounce
        else if(state.pads[LEFT_PAD].direction == DOWN || state.pads[LEFT_PAD].direction == UP) {
            state.ball.x_velocity = BALLVX_MIDLWQT;
            switch(state.pads[LEFT_PAD].direction) {
                case UP:
                    state.ball.y_velocity = -BALLVY_MIDLWQT;
                    break;
                case DOWN:
                    state.ball.y_velocity = BALLVY_MIDLWQT;
                    break;
            } 
        } 
        // normal bounce
        else {
            state.ball.x_velocity = -state.ball.x_velocity;

            // if normal bounced a top or bottom side bounce
            if(state.ball.x_velocity == -BALLVX_TBSIDE) {
                printf("speed added\n");
                state.ball.x_velocity -= BALLVX_TBSIDE_ADD;
            }
            else if(state.ball.x_velocity == BALLVX_TBSIDE) {
                printf("speed added\n");
                state.ball.x_velocity += BALLVX_TBSIDE_ADD;
            }


            if(state.ball.y_velocity < 0) 
                state.ball.y_velocity = -BALLVY_SLIGHT;
            else 
                state.ball.y_velocity = BALLVY_SLIGHT;
        }
    }

    // check collision with right side pad
    if(state.ball.x + state.ball.width > state.pads[RIGHT_PAD].x &&
        state.ball.x < state.pads[RIGHT_PAD].x + state.pads[RIGHT_PAD].width &&
        state.ball.y + state.ball.height > state.pads[RIGHT_PAD].y &&
        state.ball.y < state.pads[RIGHT_PAD].y + state.pads[RIGHT_PAD].height &&
        state.ball.bounce_turn != LEFT_PAD) {

        state.ball.bounce_turn = LEFT_PAD;
        state.ball.next_wall_bounce = -1;

        // bottom and top side bounce
        if((state.ball.y > state.pads[RIGHT_PAD].y + state.pads[RIGHT_PAD].height - 1 && state.ball.y_velocity < 0 &&
            state.pads[RIGHT_PAD].direction != DOWN) ||
            (state.ball.y + state.ball.height < state.pads[RIGHT_PAD].y + 1 && state.ball.y_velocity > 0 &&
            state.pads[RIGHT_PAD].direction != UP)) {
            
            state.ball.y_velocity = -state.ball.y_velocity;
        }
        // bottom and top side forword bounce
        else if((state.ball.y > state.pads[RIGHT_PAD].y + state.pads[RIGHT_PAD].height - 1 ||
            state.ball.y + state.ball.height < state.pads[RIGHT_PAD].y + 1) &&
            (state.pads[RIGHT_PAD].direction == DOWN || state.pads[RIGHT_PAD].direction == UP)) {
                
            switch(state.pads[RIGHT_PAD].direction) {
                case UP:
                    state.ball.x_velocity = -BALLVX_TBSIDE;
                    state.ball.y_velocity = -BALLVY_TBSIDE;
                    break;
                case DOWN:
                    state.ball.x_velocity = -BALLVX_TBSIDE;
                    state.ball.y_velocity = BALLVY_TBSIDE;
                    break;
            }
        }
        // bottom and top quarter bounce
        else if((state.pads[RIGHT_PAD].direction == DOWN || state.pads[RIGHT_PAD].direction == UP) &&
            (state.ball.y > (state.pads[RIGHT_PAD].y + state.pads[RIGHT_PAD].height) - (state.pads[RIGHT_PAD].height / 4) ||
            state.ball.y + state.ball.height < state.pads[RIGHT_PAD].y + (state.pads[RIGHT_PAD].height / 4))) {
            
            state.ball.x_velocity = -BALLVX_LOWQT;
            switch(state.pads[RIGHT_PAD].direction) {
                case UP:
                    state.ball.y_velocity = -BALLVY_LOWQT;
                    break;
                case DOWN:
                    state.ball.y_velocity = BALLVY_LOWQT;
                    break;
            } 
        }
        // top and bottom middle quarter bounce
        else if(state.pads[RIGHT_PAD].direction == DOWN || state.pads[RIGHT_PAD].direction == UP) {
            
            state.ball.x_velocity = -BALLVX_MIDLWQT;
            switch(state.pads[RIGHT_PAD].direction) {
                case UP:
                    state.ball.y_velocity = -BALLVY_MIDLWQT;
                    break;
                case DOWN:
                    state.ball.y_velocity = BALLVY_MIDLWQT;
                    break;
            } 
        }
        // normal bounce
        else {
            state.ball.x_velocity = -state.ball.x_velocity;
            if(state.ball.y_velocity < 0) 
                state.ball.y_velocity = -BALLVY_SLIGHT;
            else 
                state.ball.y_velocity = BALLVY_SLIGHT;
        }
    }

    // bounce against window upper and lower walls
    if(state.ball.y + state.ball.height > WINDOW_HEIGHT && state.ball.next_wall_bounce != UP) {
        state.ball.y_velocity = -state.ball.y_velocity;
        state.ball.next_wall_bounce = UP;
    } 
    else if(state.ball.y < 0 && state.ball.next_wall_bounce != DOWN) {
        state.ball.y_velocity = -state.ball.y_velocity;
        state.ball.next_wall_bounce = DOWN;    
    }

}

int initialize_window(void) {
    if (SDL_Init(SDL_INIT_EVERYTHING) != 0) {
        fprintf(stderr, "error in sdl init\n");
        return FALSE;
    }

    // creating window
    state.window = SDL_CreateWindow(
        "Pong Game",
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        WINDOW_WIDTH,
        WINDOW_HEIGHT,
        0
    );

    if(!state.window) {
        fprintf(stderr, "bad window\n");
        return FALSE;
    }

    // creating renderer
    state.renderer = SDL_CreateRenderer(
        state.window,
        -1,
        0
    );

    if(!state.renderer) {
        fprintf(stderr, "bad renderer");
        return FALSE;
    }

    return TRUE;
}

void process_input() {  
    SDL_Event event;
    SDL_PollEvent(&event);
    
    switch(event.type) {
        case SDL_QUIT:
            state.running = FALSE;
            break;

        case SDL_KEYDOWN:
            if(event.key.keysym.sym == SDLK_ESCAPE)
                state.running = FALSE;
            
            if(event.key.keysym.sym == SDLK_w)
                state.pads[LEFT_PAD].direction = UP;
            if(event.key.keysym.sym == SDLK_s)
                state.pads[LEFT_PAD].direction = DOWN;

            if(event.key.keysym.sym == SDLK_UP)    
                state.pads[RIGHT_PAD].direction = UP;
            if(event.key.keysym.sym == SDLK_DOWN)
                state.pads[RIGHT_PAD].direction = DOWN;

            if(event.key.keysym.sym == SDLK_r)
                resetBall(RIGHT);
            if(event.key.keysym.sym == SDLK_e)
                resetBall(LEFT);
                        
            break;
        case SDL_KEYUP:
            if(event.key.keysym.sym == SDLK_s && state.pads[LEFT_PAD].direction == DOWN)
                state.pads[LEFT_PAD].direction = -1;
            if(event.key.keysym.sym == SDLK_w && state.pads[LEFT_PAD].direction == UP)
                state.pads[LEFT_PAD].direction = -1;
            if(event.key.keysym.sym == SDLK_UP && state.pads[RIGHT_PAD].direction == UP)
                state.pads[RIGHT_PAD].direction = -1;            
            if(event.key.keysym.sym == SDLK_DOWN && state.pads[RIGHT_PAD].direction == DOWN)
                state.pads[RIGHT_PAD].direction = -1;            
            break;
    }
}
void setup() {
    // create pads
    struct pad pad1 = {
        50,
        (WINDOW_HEIGHT / 2) - (PAD_HEIGHT / 2),
        PAD_WIDTH,
        PAD_HEIGHT,
        -1,
        PAD_VELOCITY,
        0
    };
    struct pad pad2 = {
        WINDOW_WIDTH - 50,
        (WINDOW_HEIGHT / 2) - (PAD_HEIGHT / 2),
        PAD_WIDTH,
        PAD_HEIGHT,
        -1,
        PAD_VELOCITY,
        0
    };
    state.pads[LEFT_PAD] = pad1;
    state.pads[RIGHT_PAD] = pad2;

    //create ball
    struct ball ball = {
        (WINDOW_WIDTH / 2) - (BALL_WIDTH / 2),
        (WINDOW_HEIGHT / 2) - (BALL_HEIGHT / 2),
        BALL_WIDTH,
        BALL_HEIGHT,
        -BALLVX_START,
        0,
        -1
    };
    state.ball = ball;
}
void update() {
    
    // delta time (in seconds)
    int now = SDL_GetTicks(); 
    state.time.delta = (now - state.time.last_frame) / 1000.0f;
    state.time.last_frame = now;
    
    // calculates fps
    state.time.frames += 1;
    if(now - state.time.last_second > MS_PER_SEC) {
        state.time.last_second = now;
        printf("FPS: %d\n", state.time.frames);
        state.time.frames = 0;
    }

    // move pads
    for(int padn = 0; padn<PADS; padn++) {
        switch(state.pads[padn].direction) {
            case UP:
                if(state.pads[padn].y - (state.pads[padn].velocity * state.time.delta) < 0) {
                    state.pads[padn].direction = -1;
                }
                else {
                    state.pads[padn].y -= state.pads[padn].velocity * state.time.delta;
                }

                break;
            case DOWN:
                if((state.pads[padn].y + PAD_HEIGHT) + (state.pads[padn].velocity * state.time.delta) > WINDOW_HEIGHT) {
                    state.pads[padn].direction = -1;
                }
                else {
                    state.pads[padn].y += state.pads[padn].velocity * state.time.delta;
                }
                break;    
        }
    }

    // move ball 
    if(state.ball.x + state.ball.x_velocity * state.time.delta < 0) {
        state.pads[RIGHT_PAD].points += 1;
        printf("LEFT %d | %d RIGHT\n", state.pads[LEFT_PAD].points, state.pads[RIGHT_PAD].points);        
        resetBall(LEFT);
        resetPads();
    }
    else if(state.ball.x + state.ball.x_velocity * state.time.delta > WINDOW_WIDTH - state.ball.width) {
        state.pads[LEFT_PAD].points += 1;
        printf("LEFT %d | %d RIGHT\n", state.pads[LEFT_PAD].points, state.pads[RIGHT_PAD].points);
        resetBall(RIGHT);
        resetPads();
    } 
    else {
        state.ball.x += state.ball.x_velocity * state.time.delta;
    }
    state.ball.y += state.ball.y_velocity * state.time.delta;
    bounceBall();
}

void render() {
    SDL_SetRenderDrawColor(state.renderer, 0x00, 0x00, 0x00, 0xFF);
    SDL_RenderClear(state.renderer);
    
    // drawing pad
    SDL_SetRenderDrawColor(state.renderer, 0xFF, 0xFF, 0xFF, 0xFF);
    for(int padn = 0; padn<PADS; padn++) {
        SDL_Rect pad_rect = {
            state.pads[padn].x,
            state.pads[padn].y,
            state.pads[padn].width,
            state.pads[padn].height
        };

        SDL_RenderFillRect(state.renderer, &pad_rect);
    }

    // drawing ball
    SDL_SetRenderDrawColor(state.renderer, 0x9F, 0xFF, 0x9F, 0xFF);
    SDL_Rect ball_rect = {
        state.ball.x,
        state.ball.y,
        state.ball.width,
        state.ball.height,
    };
    SDL_RenderFillRect(state.renderer, &ball_rect);
    
    SDL_RenderPresent(state.renderer);
}

void destroy_window() {
    SDL_DestroyRenderer(state.renderer);
    SDL_DestroyWindow(state.window);

    SDL_Quit();
}

int main(int argc, char *argv[]) {
    // initializing sdl
    state.running = initialize_window();

    // setup envrioment, initialaze game
    setup();

    state.time.last_second = SDL_GetTicks();
    while(state.running) {
        process_input();
        update();
        render();
    }   

    // clean stuff
    destroy_window();

    return 0;
}       