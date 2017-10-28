#define WORLD_TILE_WIDTH  256
#define WORLD_TILE_HEIGHT 256
#define GRAVITY 0.2f;

#define GRASS    48
#define MARSH    49
#define WATER    50
#define WATER2   51
#define SAND     52
#define MUD      53
#define MOUNTAIN 54
#define SNOW     55
#define ST0NE    56
#define BRICK    57 
#define TREE     64

// player structures
typedef enum
{
	KEYPRESS_NONE  = 0,
	KEYPRESS_UP    = 1,
	KEYPRESS_DOWN  = 2,
	KEYPRESS_LEFT  = 4,
	KEYPRESS_RIGHT = 8
} KeyPress;

typedef enum
{
	DIR_UP = 0,
	DIR_DOWN = 3,
	DIR_LEFT = 6,
	DIR_RIGHT = 9
} Direction;

typedef enum
{
    PLAYER_ATTACK_UP,
    PLAYER_ATTACK_DOWN,
    PLAYER_ATTACK_LEFT,
    PLAYER_ATTACK_RIGHT
} AttackDirection;

typedef enum
{
	PLAYER_STATE_NONE   = 0,
	PLAYER_STATE_MOVE   = 1,
	PLAYER_STATE_ATTACK = 2,
	PLAYER_STATE_HURT   = 4,
	PLAYER_STATE_DEAD   = 8
} PlayerState;

typedef struct
{
    char* name;
    int tile_index;
    int lvl;
    int xp;
    int hp;
    int max_hp;
    float x;
    float y;
    int x_vel;
    int y_vel;
    int gold;
    int coin_throw_counter;
    int coin_throw_max;
    float base_speed;
    float speed;
    float attack_angle;
    int attack_frame_counter;
    BOOL throw_coins;
    Weapon weapon;
    Direction dir;
    AttackDirection attack_dir;
    Animation anim;
    PlayerState state;
} Player;
Player player;

static int world[WORLD_TILE_HEIGHT][WORLD_TILE_WIDTH];
static int world_collision[WORLD_TILE_HEIGHT][WORLD_TILE_WIDTH];

static int world_water_anim_counter = 0;
static int world_water_anim_counter_max = 5;

static int day_cycle_counter = 0;
static int day_cycle_direction = 1;
static int day_cycle_counter_max = 7200;
static int day_cycle_shade_amount = 0;

static void init_world(const char* path_to_world_file)
{
    FILE* fp_world = fopen(path_to_world_file,"rb");

	if (fp_world == NULL)
		return;

    int c;
	unsigned char uc;

	for (int j = 0; j < WORLD_TILE_HEIGHT; ++j)
	{
		for(int i = 0; i < WORLD_TILE_WIDTH; ++i)
		{
            c = fgetc(fp_world);
            if(c == EOF) return;

			uc = (unsigned char)c;

			world[j][i] = uc;

            switch(uc)
            {
                case GRASS: world_collision[i][j] = 1; break;
                case SAND:  world_collision[i][j] = 2; break;
                case MUD:   world_collision[i][j] = 3; break;
                case WATER: world_collision[i][j] = 4; break;
                case MOUNTAIN: world_collision[i][j] = 5; break;
                case TREE:  world_collision[i][j] = 5; break;
            }
        }
    }

    fclose(fp_world);
}

static void update_world()
{
    world_water_anim_counter++;

	if (world_water_anim_counter == world_water_anim_counter_max)
	{
		world_water_anim_counter = 0;
		for(int j = 0; j < WORLD_TILE_HEIGHT; ++j)
		{
			for(int i = 0; i < WORLD_TILE_WIDTH; ++i)
			{
				if(world[i][j] == WATER || world[i][j] == WATER2)
				{
					// animate
                    if(world[i][j] == WATER) 
                        world[i][j] = WATER2;
                    else
                        world[i][j] = WATER;
                }
            }
        }
    }
    
    // update day cycle
    day_cycle_counter += day_cycle_direction;
    day_cycle_shade_amount = 10.0f*((float)day_cycle_counter/DAY_CYCLE_COUNTER_MAX);

    if(day_cycle_counter == DAY_CYCLE_COUNTER_MAX || day_cycle_counter == 0)
        day_cycle_direction *= -1;

}
static void draw_world(POINT camera, int shade_amount)
{
    int start_x = camera.x / TILE_WIDTH;
    int start_y = camera.y / TILE_HEIGHT;

    int end_x = start_x + (buffer_width/TILE_WIDTH) + 1;
    int end_y = start_y + (buffer_height/TILE_HEIGHT) + 1;

    if (end_x > WORLD_TILE_WIDTH)  end_x = WORLD_TILE_WIDTH;
    if (end_y > WORLD_TILE_HEIGHT) end_y = WORLD_TILE_HEIGHT;

    for(int j = start_y; j < end_y; ++j)
    {
        for(int i = start_x; i < end_x; ++i)
        {
            draw_tile(i*TILE_WIDTH - camera.x,j*TILE_HEIGHT - camera.y,world[j][i],shade_amount);
        }
    }
}

