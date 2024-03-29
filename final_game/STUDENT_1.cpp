#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_net.h>
#include <stdio.h>
#include <bits/stdc++.h>
#include <fstream>
#include <SDL2/SDL_mixer.h>
using namespace std;


const int TILE_WIDTH = 32;
const int TILE_HEIGHT = 32;
const int tot_tiles = 60000;
const int tile_sprite1 = 324;
const int tile_sprite2 = 196;
const int tile_sprite3 = 16384;
const int tile_sprite4 = 99;
const int tile_sprite5 = 6;
const int tile_sprite6 = 384;
const int tile_sprite7 = 4096;
const int tile_sprite = tile_sprite1 + tile_sprite2 + tile_sprite3 + tile_sprite4 + tile_sprite5 + tile_sprite6 + tile_sprite7;

vector<int> walkingTiles{7, 8, 20, 21, 22, 23, 34, 35, 36, 37, 49, 50, 62, 63, 64, 65, 76, 77, 78, 79, 90, 91, 92, 93, 104, 105, 106, 107, 14742};

const int sc_width = 1280;
const int sc_height = 720;

const int lvl_width = 9600;
const int lvl_height = 6400;


const int FRAMES_WALKING = 16;
SDL_Rect charOneClips[FRAMES_WALKING];
SDL_Rect charTwoClips[FRAMES_WALKING];

int frame = 0;
int win = 0;

SDL_Rect gate6 = {410,0,64,64};

//Texture wrapper class
class LTexture
{
	public:
		//Initializes variables
		LTexture();

		//Deallocates memory
		~LTexture();

		//Loads image at specified path
		bool loadFromFile( std::string path );
		
		#if defined(SDL_TTF_MAJOR_VERSION)
		//Creates image from font string
		bool loadFromRenderedText( std::string textureText, SDL_Color textColor );
		#endif

		//Deallocates texture
		void free();

		//Set color modulation
		void setColor( Uint8 red, Uint8 green, Uint8 blue );

		//Set blending
		void setBlendMode( SDL_BlendMode blending );

		//Set alpha modulation
		void setAlpha( Uint8 alpha );
		
		//Renders texture at given point
		void render( int x, int y, SDL_Rect* clip = NULL, double angle = 0.0, SDL_Point* center = NULL, SDL_RendererFlip flip = SDL_FLIP_NONE );

		//Gets image dimensions
		int getWidth();
		int getHeight();

		SDL_Texture* getTexture(){
			return mTexture;
		}

	private:
		//The actual hardware texture
		SDL_Texture* mTexture;

		//Image dimensions
		int mWidth;
		int mHeight;
};



//The tile
class Tile
{
    public:
		//Initializes position and type
		Tile( int x, int y, int tileType );

		//Shows the tile
		void render( SDL_Rect& camera );

		//Get the tile type
		int getType();

		//Get the collision box
		SDL_Rect getBox();

    private:
		//The attributes of the tile
		SDL_Rect mBox;

		//The tile type
		int type_of_Tile;
};


//The dot that will move around on the screen
class Dot
{
    public:
		//The dimensions of the dot
		static const int DOT_WIDTH = 20;
		static const int DOT_HEIGHT = 20;

		//Maximum axis velocity of the dot
		static const int DOT_VEL = 3;

		//Initializes the variables
		Dot();

		//Takes key presses and adjusts the dot's velocity
		void handleEvent( SDL_Event& e );

		//Moves the dot and check collision against tiles
		void move( Tile *tiles[] );

		//Centers the camera over the dot
		void setCamera( SDL_Rect& camera );

		//Shows the dot on the screen
		void render( SDL_Rect& camera );

		SDL_Rect getBox();

    private:
		//Collision box of the dot
		SDL_Rect mBox;

		//The velocity of the dot
		int mVelX, mVelY;
};


//Starts up SDL and creates window
bool init();

//Loads media
bool loadMedia( Tile* tiles[], Tile* tiles2[]);

//Frees media and shuts down SDL
void close( Tile* tiles[] );

//Box collision detector
bool checkCollision( SDL_Rect a, SDL_Rect b );

//Checks collision box against set of tiles
bool touchesRoad( SDL_Rect box, Tile* tiles[] );

//Sets tiles from tile map
bool setTiles( Tile *tiles[], string file);

//The window we'll be rendering to
SDL_Window* gWindow = NULL;

//The window renderer
SDL_Renderer* gRenderer = NULL;

//Scene textures
LTexture textureDOT;
LTexture textureCHAR_ONE;
LTexture textureCHAR_TWO;
LTexture textureIMAGE_ONE;
LTexture textureIMAGE_TWO;
LTexture textureIMAGE_THREE;
LTexture textureIMAGE_FOUR;
LTexture textureIMAGE_FIVE;
LTexture textureIMAGE_SIX;
LTexture textureIMAGE_SEVEN;
LTexture winscreenTexture;
LTexture lostscreenTexture;
SDL_Rect gTileClips1[ tile_sprite1 ];
SDL_Rect gTileClips2[ tile_sprite2 ];
SDL_Rect gTileClips3[ tile_sprite3 ];
SDL_Rect gTileClips4[ tile_sprite4 ];
SDL_Rect gTileClips5[ tile_sprite5 ];
SDL_Rect gTileClips6[ tile_sprite6 ];
SDL_Rect gTileClips7[ tile_sprite7 ];

LTexture::LTexture()
{
	//Initialize
	mTexture = NULL;
	mWidth = 0;
	mHeight = 0;
}

LTexture::~LTexture()
{
	//Deallocate
	free();
}

bool LTexture::loadFromFile( std::string path )
{
	//Get rid of preexisting texture
	free();

	//The final texture
	SDL_Texture* newTexture = NULL;

	//Load image at specified path
	SDL_Surface* loadedSurface = IMG_Load( path.c_str() );
	if( loadedSurface == NULL )
	{
		printf( "Unable to load image %s! SDL_image Error: %s\n", path.c_str(), IMG_GetError() );
	}
	else
	{
		if(path.compare("assests/character.bmp"))
		SDL_SetColorKey( loadedSurface, SDL_TRUE, SDL_MapRGB( loadedSurface->format, 0xFE, 0xFE, 0xFE ) );

		//Create texture from surface pixels
        newTexture = SDL_CreateTextureFromSurface( gRenderer, loadedSurface );
		if( newTexture == NULL )
		{
			printf( "Unable to create texture from %s! SDL Error: %s\n", path.c_str(), SDL_GetError() );
		}
		else
		{
			//Get image dimensions
			mWidth = loadedSurface->w;
			mHeight = loadedSurface->h;
		}

		//Get rid of old loaded surface
		SDL_FreeSurface( loadedSurface );
	}

	//Return success
	mTexture = newTexture;
	return mTexture != NULL;
}

#if defined(SDL_TTF_MAJOR_VERSION)
bool LTexture::loadFromRenderedText( std::string textureText, SDL_Color textColor )
{
	//Get rid of preexisting texture
	free();

	//Render text surface
	SDL_Surface* textSurface = TTF_RenderText_Solid( gFont, textureText.c_str(), textColor );
	if( textSurface != NULL )
	{
		//Create texture from surface pixels
        mTexture = SDL_CreateTextureFromSurface( gRenderer, textSurface );
		if( mTexture == NULL )
		{
			printf( "Unable to create texture from rendered text! SDL Error: %s\n", SDL_GetError() );
		}
		else
		{
			//Get image dimensions
			mWidth = textSurface->w;
			mHeight = textSurface->h;
		}

		//Get rid of old surface
		SDL_FreeSurface( textSurface );
	}
	else
	{
		printf( "Unable to render text surface! SDL_ttf Error: %s\n", TTF_GetError() );
	}

	
	//Return success
	return mTexture != NULL;
}
#endif

void LTexture::free()
{
	//Free texture if it exists
	if( mTexture != NULL )
	{
		SDL_DestroyTexture( mTexture );
		mTexture = NULL;
		mWidth = 0;
		mHeight = 0;
	}
}

void LTexture::setColor( Uint8 red, Uint8 green, Uint8 blue )
{
	//Modulate texture rgb
	SDL_SetTextureColorMod( mTexture, red, green, blue );
}

void LTexture::setBlendMode( SDL_BlendMode blending )
{
	//Set blending function
	SDL_SetTextureBlendMode( mTexture, blending );
}
		
void LTexture::setAlpha( Uint8 alpha )
{
	//Modulate texture alpha
	SDL_SetTextureAlphaMod( mTexture, alpha );
}

void LTexture::render( int x, int y, SDL_Rect* clip, double angle, SDL_Point* center, SDL_RendererFlip flip )
{
	//Set rendering space and render to screen
	SDL_Rect renderQuad = { x, y, mWidth, mHeight };

	//Set clip rendering dimensions
	if( clip != NULL )
	{
		renderQuad.w = clip->w;
		renderQuad.h = clip->h;
	}

	//Render to screen
	SDL_RenderCopyEx( gRenderer, mTexture, clip, &renderQuad, angle, center, flip );
}

int LTexture::getWidth()
{
	return mWidth;
}

int LTexture::getHeight()
{
	return mHeight;
}

Tile::Tile( int x, int y, int tileType )
{
    //Get the offsets
    mBox.x = x;
    mBox.y = y;

    //Set the collision box
    mBox.w = TILE_WIDTH;
    mBox.h = TILE_HEIGHT;

    //Get the tile type
    type_of_Tile = tileType;
}

void Tile::render( SDL_Rect& camera )
{
    //If the tile is on screen
    if( checkCollision( camera, mBox ) )
    {
        //Show the tile
		if (type_of_Tile == 0)
			// textureIMAGE_ONE.render( mBox.x - camera.x, mBox.y - camera.y, &gTileClips1[232] );
			return;

		if (type_of_Tile > 0 && type_of_Tile <= 324)
        	textureIMAGE_ONE.render( mBox.x - camera.x, mBox.y - camera.y, &gTileClips1[ type_of_Tile - 1] );

		else if (type_of_Tile > 324 && type_of_Tile <= 520)
			textureIMAGE_TWO.render( mBox.x - camera.x, mBox.y - camera.y, &gTileClips2[ type_of_Tile - 325] );
		
		else if (type_of_Tile > 520 && type_of_Tile <= 16904)
			textureIMAGE_THREE.render( mBox.x - camera.x, mBox.y - camera.y, &gTileClips3[ type_of_Tile - 521 ] );
		
		else if (type_of_Tile > 16904 && type_of_Tile <= 17003)
			textureIMAGE_FOUR.render(mBox.x - camera.x, mBox.y - camera.y, &gTileClips4[ type_of_Tile - 16905 ] );

		else if (type_of_Tile > 17003 && type_of_Tile <= 17009)
			textureIMAGE_FIVE.render(mBox.x - camera.x, mBox.y - camera.y, &gTileClips5[ type_of_Tile - 17004 ] );

		else if (type_of_Tile > 17009 && type_of_Tile <= 17393)
			textureIMAGE_SIX.render(mBox.x - camera.x, mBox.y - camera.y, &gTileClips6[ type_of_Tile - 17010 ] );

		else
			textureIMAGE_SEVEN.render(mBox.x - camera.x, mBox.y - camera.y, &gTileClips7[ type_of_Tile - 17394 ] );
		
	}
}

int Tile::getType()
{
    return type_of_Tile;
}

SDL_Rect Tile::getBox()
{
    return mBox;
}

Dot::Dot()
{
    //Initialize the collision box
    mBox.x = 9290;
    mBox.y = 3520;

	mBox.w = DOT_WIDTH;
	mBox.h = DOT_HEIGHT;

    //Initialize the velocity
    mVelX = 0;
    mVelY = 0;
}

void Dot::handleEvent( SDL_Event& e )
{
    //If a key was pressed
	if( e.type == SDL_KEYDOWN && e.key.repeat == 0 )
    {
        //Adjust the velocity
        switch( e.key.keysym.sym )
        {
            case SDLK_UP: mVelY -= DOT_VEL; break;
            case SDLK_DOWN: mVelY += DOT_VEL; break;
            case SDLK_LEFT: mVelX -= DOT_VEL; break;
            case SDLK_RIGHT: mVelX += DOT_VEL; break;
        }
    }
    //If a key was released
    else if( e.type == SDL_KEYUP && e.key.repeat == 0 )
    {
        //Adjust the velocity
        switch( e.key.keysym.sym )
        {
            case SDLK_UP: mVelY += DOT_VEL; break;
            case SDLK_DOWN: mVelY -= DOT_VEL; break;
            case SDLK_LEFT: mVelX += DOT_VEL; break;
            case SDLK_RIGHT: mVelX -= DOT_VEL; break;
        }
    }
}

void Dot::move( Tile *tiles[] )
{
    //Move the dot left or right
    mBox.x += mVelX;

    if( ( mBox.x < 0 ) || ( mBox.x + DOT_WIDTH > lvl_width ) || !touchesRoad(mBox, tiles))
    {
        //move back
        mBox.x -= mVelX;
    }

    mBox.y += mVelY;

    if( ( mBox.y < 0 ) || ( mBox.y + DOT_HEIGHT > lvl_height ) || !touchesRoad(mBox, tiles))
    {
        //move back
        mBox.y -= mVelY;
    }
}

void Dot::setCamera( SDL_Rect& camera )
{
	//Center the camera over the dot
	camera.x = ( mBox.x + DOT_WIDTH / 2 ) - sc_width / 2;
	camera.y = ( mBox.y + DOT_HEIGHT / 2 ) - sc_height / 2;

	//Keep the camera in bounds
	if( camera.x < 0 )
	{ 
		camera.x = 0;
	}
	if( camera.y < 0 )
	{
		camera.y = 0;
	}
	if( camera.x > lvl_width - camera.w )
	{
		camera.x = lvl_width - camera.w;
	}
	if( camera.y > lvl_height - camera.h )
	{
		camera.y = lvl_height - camera.h;
	}
}

SDL_Rect Dot :: getBox(){

	return mBox;

}


void Dot::render( SDL_Rect& camera )
{
    //Show the dot
	textureDOT.render( mBox.x - camera.x, mBox.y - camera.y );
}

class Player{
	public:
	SDL_Rect charBox;
	int id, direction, mVelx, mVely;
	static const int DOT_VEL = 10;
	Player(int playerId){
		direction = 0;
		mVelx = 0;
		mVely = 0;
		id = playerId;
		charBox.x = 9290;
		charBox.y = 3520;
		charBox.w = 60;
		charBox.h = 60;
		
	}

	void render(SDL_Renderer* gRenderer,SDL_Rect* camera, int frame){
		SDL_Rect newrect = {charBox.x-camera->x,charBox.y-camera->y,charBox.w,charBox.h};

		if (id == 1)
			SDL_RenderCopy(gRenderer,textureCHAR_ONE.getTexture(),&charOneClips[direction*4+frame],&newrect);
		else if (id == 2)
			SDL_RenderCopy(gRenderer,textureCHAR_TWO.getTexture(),&charTwoClips[direction*4+frame],&newrect);
	}

	void handleEvent(SDL_Event& e){
		if( e.type == SDL_KEYDOWN && e.key.repeat == 0 )
		{

			switch( e.key.keysym.sym )
			{
				case SDLK_UP: mVely -= DOT_VEL; direction = 3; break;
				case SDLK_DOWN: mVely += DOT_VEL; direction = 0; break;
				case SDLK_LEFT: mVelx -= DOT_VEL; direction = 1; break;
				case SDLK_RIGHT: mVelx += DOT_VEL; direction = 2; break;
			}
		}
		//If a key was released
		else if( e.type == SDL_KEYUP && e.key.repeat == 0 )
		{
			//Adjust the velocity
			switch( e.key.keysym.sym )
			{
				case SDLK_UP: mVely += DOT_VEL; break;
				case SDLK_DOWN: mVely -= DOT_VEL; break;
				case SDLK_LEFT: mVelx += DOT_VEL; break;
				case SDLK_RIGHT: mVelx -= DOT_VEL; break;
			}
		}
	}

	void move(Tile *tiles[]){
		 charBox.x += mVelx;

		if( ( charBox.x < 0 ) || ( charBox.x + charBox.w > lvl_width ) || !touchesRoad(charBox, tiles))
		{
			//move back
			charBox.x -= mVelx;
		}

		charBox.y += mVely;

		if( ( charBox.y < 0 ) || ( charBox.y + charBox.h > lvl_height ) || !touchesRoad(charBox, tiles))
		{
			//move back
			charBox.y -= mVely;
		}

	}

	void setCamera(SDL_Rect& camera){
		//Center the camera over the dot
		camera.x = ( charBox.x + charBox.w / 2 ) - sc_width / 2;
		camera.y = ( charBox.y + charBox.h / 2 ) - sc_height / 2;

		if( camera.x < 0 )
		{ 
			camera.x = 0;
		}
		if( camera.y < 0 )
		{
			camera.y = 0;
		}
		if( camera.x > lvl_width - camera.w )
		{
			camera.x = lvl_width - camera.w;
		}
		if( camera.y > lvl_height - camera.h )
		{
			camera.y = lvl_height - camera.h;
		}

	}

	SDL_Rect getCharRect(){
        return charBox;
    }

    bool character_collision(SDL_Rect camera, SDL_Rect fromserver){
        charBox = fromserver;
        return checkCollision(charBox,camera);
    }

	int getDirection(){
		return direction;
	}

	void changedirection(int dir){
		direction = dir;
	}

};

bool init()
{
	//Initialization flag
	bool success = true;

	//Initialize SDL
	if( SDL_Init( SDL_INIT_VIDEO ) < 0 )
	{
		printf( "SDL could not initialize! SDL Error: %s\n", SDL_GetError() );
		success = false;
	}
	else
	{
		//Set texture filtering to linear
		if( !SDL_SetHint( SDL_HINT_RENDER_SCALE_QUALITY, "1" ) )
		{
			printf( "Warning: Linear texture filtering not enabled!" );
		}

		//Create window
		gWindow = SDL_CreateWindow( "STUDENT_1", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, sc_width, sc_height, SDL_WINDOW_SHOWN );
		if( gWindow == NULL )
		{
			printf( "Window could not be created! SDL Error: %s\n", SDL_GetError() );
			success = false;
		}
		else
		{
			//Create renderer for window
			gRenderer = SDL_CreateRenderer( gWindow, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC );
			if( gRenderer == NULL )
			{
				printf( "Renderer could not be created! SDL Error: %s\n", SDL_GetError() );
				success = false;
			}
			else
			{
				//Initialize renderer color
				SDL_SetRenderDrawColor( gRenderer, 0xFF, 0xFF, 0xFF, 0xFF );

				//Initialize PNG loading
				int imgFlags = IMG_INIT_PNG;
				if( !( IMG_Init( imgFlags ) & imgFlags ) )
				{
					printf( "SDL_image could not initialize! SDL_image Error: %s\n", IMG_GetError() );
					success = false;
				}
			}
		}
	}

	return success;
}

bool loadMedia( Tile* tiles[], Tile* tiles2[])
{
	//Loading success flag
	bool success = true;

	if(!lostscreenTexture.loadFromFile("poster+music/lost.png")){
		printf( "Failed to lost screen texture!\n" );
		success = false;
	}

	if(!winscreenTexture.loadFromFile("poster+music/won.png")){
		printf( "Failed to win screen texture!\n" );
		success = false;
	}

	//Load dot texture
	if( !textureDOT.loadFromFile( "assets/dot.bmp" ) )
	{
		printf( "Failed to load dot texture!\n" );
		success = false;
	}

	if( !textureCHAR_ONE.loadFromFile("assets/misty.png")){
		printf( "Failed to load walking animation texture!\n" );
        success = false;
	}

	else{

		int x = 0, y = 0, cnt = 0;

		for (int i = 0; i < 4; i++){
			for (int j = 0; j < 4; j++){
				charOneClips[cnt] = {x, y, 68, 65};
				x += 68;
				cnt++;
			}
			x = 0;
			y += 72;
		}
        
	}

	if (!textureCHAR_TWO.loadFromFile("assets/zblue.png")){
		printf( "Failed to load walking animation texture!\n" );
        success = false;
	}
	else{
		int x = 0, y = 0, cnt = 0;

		for (int i = 0; i < 4; i++){
			for (int j = 0; j < 4; j++){
				charTwoClips[cnt] = {x, y, 64, 64};
				x += 64;
				cnt++;
			}
			x = 0;
			y += 64;
		}
	}

	//Load tile texture
	if( !textureIMAGE_ONE.loadFromFile( "assets/grass.jpeg" ) )
	{
		printf( "Failed to load tile set texture!\n" );
		success = false;
	}

	if( !textureIMAGE_TWO.loadFromFile( "assets/outdoors.png" ) )
	{
		printf( "Failed to load tile set texture!\n" );
		success = false;
	}

	if( !textureIMAGE_THREE.loadFromFile( "assets/main.png" ) )
	{
		printf( "Failed to load tile set texture!\n" );
		success = false;
	}

	if( !textureIMAGE_FOUR.loadFromFile( "assets/vehicles.png" ) )
	{
		printf( "Failed to load tile set texture!\n" );
		success = false;
	}


	if( !textureIMAGE_FIVE.loadFromFile( "assets/font1.png" ) )
	{
		printf( "Failed to load tile set texture!\n" );
		success = false;
	}


	if( !textureIMAGE_SIX.loadFromFile( "assets/buttons.png" ) )
	{
		printf( "Failed to load tile set texture!\n" );
		success = false;
	}


	if( !textureIMAGE_SEVEN.loadFromFile( "assets/blue-spritesheet.png" ) )
	{
		printf( "Failed to load tile set texture!\n" );
		success = false;
	}


	//Load tile map
	if( !setTiles( tiles, "maps/layer1.txt") )
	{
		printf( "Failed to load tile set for layer 1!\n" );
		success = false;
	}

	if( !setTiles( tiles2, "maps/layer2.txt") )
	{
		printf( "Failed to load tile set for layer 2!\n" );
		success = false;
	}


	return success;
}

void close( Tile* tiles[] )
{
	//Deallocate tiles
	for( int i = 0; i < tot_tiles; ++i )
	{
		 if( tiles[ i ] != NULL )
		 {
			delete tiles[ i ];
			tiles[ i ] = NULL;
		 }
	}

	//Free loaded images
	textureDOT.free();
	textureIMAGE_ONE.free();
	textureIMAGE_TWO.free();
	textureIMAGE_THREE.free();
	textureIMAGE_FOUR.free();
	textureIMAGE_FIVE.free();
	textureIMAGE_SIX.free();
	textureIMAGE_SEVEN.free();
	textureCHAR_ONE.free();
	textureCHAR_TWO.free();


	//Destroy window	
	SDL_DestroyRenderer( gRenderer );
	SDL_DestroyWindow( gWindow );
	gWindow = NULL;
	gRenderer = NULL;

	//Quit SDL subsystems
	IMG_Quit();
	SDL_Quit();
}

bool checkCollision( SDL_Rect a, SDL_Rect b )
{
    //The sides of the rectangles
    int leftA, leftB;
    int rightA, rightB;
    int topA, topB;
    int bottomA, bottomB;

    //Calculate the sides of rect A
    leftA = a.x;
    rightA = a.x + a.w;
    topA = a.y;
    bottomA = a.y + a.h;

    //Calculate the sides of rect B
    leftB = b.x;
    rightB = b.x + b.w;
    topB = b.y;
    bottomB = b.y + b.h;

    //If any of the sides from A are outside of B
    if( bottomA <= topB )
    {
        return false;
    }

    if( topA >= bottomB )
    {
        return false;
    }

    if( rightA <= leftB )
    {
        return false;
    }

    if( leftA >= rightB )
    {
        return false;
    }

    //If none of the sides from A are outside B
    return true;
}

bool setTiles( Tile* tiles[], string file)
{
	//Success flag
	bool tilesLoaded = true;

    //The tile offsets
    int x = 0, y = 0;

    //Open the map
    std::ifstream map(file);

    //If the map couldn't be loaded
    if( map.fail() )
    {
		printf( "Unable to load map file!\n" );
		tilesLoaded = false;
    }
	else
	{
		//Initialize the tiles
		for( int i = 0; i < tot_tiles; ++i )
		{
			//Determines what kind of tile will be made
			int tileType = -1;

			//Read tile from map file
			map >> tileType;

			//If the was a problem in reading the map
			if( map.fail() )
			{
				//Stop loading map
				printf( "Error loading map: Unexpected end of file!\n" );
				tilesLoaded = false;
				break;
			}

			//If the number is a valid tile number
			if( ( tileType >= 0 ) && ( tileType < tile_sprite ) )
			{
				tiles[ i ] = new Tile( x, y, tileType );
				// cout << x << " " << y << endl;
			}
			//If we don't recognize the tile type
			else
			{
				//Stop loading map
				printf( "Error loading map: Invalid tile type at %d!\n", i );
				//cout << "Tile Type: " << tileType << endl;
				tilesLoaded = false;
				break;
			}

			//Move to next tile spot
			x += TILE_WIDTH;

			//If we've gone too far
			if( x >= lvl_width )
			{
				//Move back
				x = 0;

				//Move to the next row
				y += TILE_HEIGHT;
			}
		}
		
		//Clip the sprite sheet
		if( tilesLoaded )
		{
			int cnt1 = 0, x1 = 0, y1 = 0;
			for (int i = 0; i < 18; i++){
				for (int j=0; j < 18; j++){
					gTileClips1[cnt1] = {x1, y1, TILE_WIDTH, TILE_HEIGHT};
					x1 += 32;
					cnt1++; 
				}
				y1 += 32;
				x1 = 0;
			}

			int cnt2 = 0, x2 = 0, y2 = 0;
			for (int i = 0; i < 14; i++){
				for (int j=0; j < 14; j++){
					gTileClips2[cnt2] = {x2, y2, TILE_WIDTH, TILE_HEIGHT};
					x2 += 32;
					cnt2++; 
				}
				y2 += 32;
				x2 = 0;
			}

			int cnt3 = 0, x3 = 0, y3 = 0;
			for (int i = 0; i < 128; i++){
				for (int j=0; j < 128; j++){
					gTileClips3[cnt3] = {x3, y3, TILE_WIDTH, TILE_HEIGHT};
					x3 += 32;
					cnt3++; 
				}
				y3 += 32;
				x3 = 0;
			}

			int cnt4 = 0, x4 = 0, y4 = 0;
			for (int i = 0; i < 9; i++){
				for (int j=0; j < 11; j++){
					gTileClips4[cnt4] = {x4, y4, TILE_WIDTH, TILE_HEIGHT};
					x4 += 32;
					cnt4++; 
				}
				y4 += 32;
				x4 = 0;
			}

			int cnt5 = 0, x5 = 0, y5 = 0;
			for (int i = 0; i < 2; i++){
				for (int j=0; j < 3; j++){
					gTileClips5[cnt5] = {x5, y5, TILE_WIDTH, TILE_HEIGHT};
					x5 += 32;
					cnt5++; 
				}
				y5 += 32;
				x5 = 0;
			}


			int cnt6 = 0, x6 = 0, y6 = 0;
			for (int i = 0; i < 48; i++){
				for (int j=0; j < 8; j++){
					gTileClips6[cnt6] = {x6, y6, TILE_WIDTH, TILE_HEIGHT};
					x6 += 32;
					cnt6++; 
				}
				y6 += 32;
				x6 = 0;
			}

			int cnt7 = 0, x7 = 0, y7 = 0;
			for (int i = 0; i < 64; i++){
				for (int j=0; j < 64; j++){
					gTileClips7[cnt7] = {x7, y7, TILE_WIDTH, TILE_HEIGHT};
					x7 += 32;
					cnt7++; 
				}
				y7 += 32;
				x7 = 0;
			}



		}
	}

    //Close the file
    map.close();

    //If the map was loaded fine
    return tilesLoaded;
}

bool touchesRoad( SDL_Rect box, Tile* tiles[] )
{
    //Go through the tiles
    for( int i = 0; i < tot_tiles; ++i )
    {
        //If the tile is a wall type tile
        int type = tiles[i]->getType() - 325;
		SDL_Rect tet = tiles[i]->getBox();


		if(find(walkingTiles.begin(), walkingTiles.end(), type) != walkingTiles.end())
        {
            if( checkCollision( box, tiles[ i ]->getBox() ) )
            {
                return true;
            }
        }
    }

    //If no wall tiles were touched
    return false;
}

int main( int argc, char* args[] )
{
	//Start up SDL and create window
	if( !init() )
	{
		printf( "Failed to initialize!\n" );
	}
	else
	{
		//The level tiles
		Tile* tileSet[ tot_tiles ];
		Tile* tileSet2[tot_tiles];

		//Networking
		int inputData[] = {0,0,0,0,0,0,0}, outputData[] = {0,0,0,0,0,0,0};

		IPaddress ip;
        SDLNet_ResolveHost(&ip, "127.0.0.1", 1234);
        TCPsocket client = SDLNet_TCP_Open(&ip);

		//Load media
		if (client!=NULL){
			if( !loadMedia( tileSet, tileSet2) )
			{
				printf( "Failed to load media!\n" );
			}
			else
			{	
				//Main loop flag
				bool quit = false;

				//Event handler
				SDL_Event e;

				Player student(1);
				Player student_2(2);

				//Level camera
				SDL_Rect camera = { 0, 0, sc_width, sc_height };
				SDL_Rect fromserver;

				bool gamescreen = true, endscreen = false;


				//While application is running
				while( !quit )
				{
					//Handle events on queue
					while( SDL_PollEvent( &e ) != 0 )
					{
						//User requests quit
						if( e.type == SDL_QUIT )
						{
							quit = true;
							outputData[0] = -1;
                            SDLNet_TCP_Send(client, outputData, 28);
						}

					
						student.handleEvent(e);

						if(e.type == SDL_KEYDOWN){
							frame++;
							if(frame >= 16){
								frame = 0;
							}
						}
					}

					//Clear screen
					SDL_SetRenderDrawColor( gRenderer, 0xFF, 0xFF, 0xFF, 0xFF );
					SDL_RenderClear( gRenderer );


					

					student.move(tileSet2);
					SDL_Rect curpos = student.getCharRect();
                    outputData[0] = curpos.x;
                    outputData[1] = curpos.y;
					//cout << curpos.x << " " << curpos.y << endl;
                    outputData[2] = curpos.w;
                    outputData[3] = curpos.h;
					outputData[4] = student.getDirection();
					outputData[5] = frame/4;
					outputData[6] = win;
                    SDLNet_TCP_Send(client, outputData, 28);
					student.setCamera(camera);

					
					//Render level 1
					for( int i = 0; i < tot_tiles; ++i )
					{
						tileSet[ i ]->render( camera );
					}

					//Render level 2
					for( int i = 0; i < tot_tiles; ++i )
					{
						tileSet2[ i ]->render( camera );
					}

					student.render(gRenderer, &camera, frame/4);

					SDLNet_TCP_Recv(client,inputData,28);
                    fromserver = {inputData[0], inputData[1], inputData[2], inputData[3]};


                    if(student_2.character_collision(camera,fromserver)){
						student_2.changedirection(inputData[4]);
                        student_2.render(gRenderer, &camera, inputData[5]);
                    }

					if(checkCollision(student.getCharRect(), gate6)){
						win = 1;
					}
					else if(inputData[6]){
						win = inputData[6];
					}

					if(win==1){
						SDL_RenderCopy(gRenderer,winscreenTexture.getTexture(),NULL,NULL );
					}
					else if(win == 2){
						SDL_RenderCopy(gRenderer,lostscreenTexture.getTexture(),NULL,NULL);
					}

					//cout << win << endl;
					

					//Update screen
					SDL_RenderPresent( gRenderer );


				}
			}

			SDLNet_TCP_Close(client);

		}
		SDLNet_Quit();
		close( tileSet );
	}

	return 0;
}
