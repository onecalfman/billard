//====================================================================
// Simulation dynamischer Systeme mit PLAN
//====================================================================
// Projektbeschreibung:
//
//====================================================================
//Fouls:
//1.falschen Kugeln ins Loch
//2.falschen Kugeln st��t
//
//
//
//====================================================================
#include <vcl.h>
#pragma hdrstop
#include "Plan.h"
#include "Data.h"
#include <iostream>
#define log(x) std::cout << x << std::endl;
#define N 16

static double Stocklang = 1100.0;
static double Stockradius = 30.0;
static double Stockanfangsort = -800.0;

static double my = 0.01;         // Rollreibung
//static double r = 30.0;          // Kugelradius
static double ForceScale = 0.06; // Skalierungskonstante f��1r die Kraft bei Stoss mit dem Queue

static double leftBorder = -1230.0;
static double rightBorder = 1230.0;
static double topBorder = 595.0;
static double bottomBorder = -595.0;

static bool Foul;                // ob es ein Foul eintritt
static bool FoulK0;
static bool Treffer;              // ob der weisse Kugel anderen Kugeln trifft
static bool insloch;             // gibt es ein Kugel, der in dieser Runde in das Loch gegangen ist
static bool Spieler1;            // ob der Spieler 1 jetzt spielen
static bool Spieler2;            // ob der Spieler 2 jetzt spielen
static bool Reihe;               // ob es in Game ist
static bool erststoss;           // ob der wesse Kugel anderen schon ein mal gestosst
static int Ordnung;              // Die Ornung von Kugeln fuer beide Spielern
static bool Gameon;              // this Game is not on
static bool gameover;
static bool gamewin;
int counter;
int ProductionError;
AnsiString Name1Input;
PlanString Name1Text;
AnsiString Name2Input;
PlanString Name2Text;
int ChangeName;
int FrictionMode = 2;

real dist(real x1, real y1, real x2, real y2) {
	return sqrt(pow(x1 - x2,2) + pow(y1 - y2, 2));
}

int randInt(int min, int max){
	return rand()%(max-min + 1) + min;
}

int dist(int x1, int y1, int x2, int y2) {
	return sqrt(pow(x1 - x2,2) + pow(y1 - y2, 2));
}

double norm(TVektor a) {
	//if(a[0] < 0.0001 && a[1] < 0.0001) return 0.0;
	return sqrt(a[0]*a[0] + a[1]*a[1]);
}

TVektor eye(TVektor a){
	double norm_a = norm(a);
	if(! norm_a) return TVektor(0.0,0.0);
	return a/norm(a);
}

double dot(TVektor a, TVektor b) {
	real res = 0.0;
	for(int i = 0; i < a.Zeilen(); i++) {
		res += a[i] * b[i];
	}
	return res;
}

double min(double a, double b){
	if(a <= b) return a;
	return b;
}

double max(double a, double b){
	if(a <= b) return b;
	return a;
}

TVektor min(TVektor a, TVektor b){
	if(norm(a) <= norm(b)) return a;
	return b;
}

TVektor max(TVektor a, TVektor b){
	if(norm(a) <= norm(b)) return b;
	return a;
}

void vlog(TVektor v) {
	double a = v[0];
	double b = v[1];
	printf("x: %8.31e, y: %8.31e\n", a, b);
}


class Loch {
	public:
		int n;
		double r;
		TVektor pos;

		void init(int n, double x, double y) {
			this->n = n;
			this->pos = TVektor(x,y);
			this->r = 60;
		}
};

class Kugel {
	private:

	public:
                int n;         // Nummer der Kugel
		real r;        // Radius
		TVektor pos;   // Position pos[0] = x, pos[1] = y
		TVektor next;  // Posistion in der N?chsten Iteration
		TVektor v;     // Geschwindigkeit
		double m;      // Masse
		bool inGame;   // Ist Kugel im Spiel
		TColor color;

		void init(int n, real x, real y, TColor color) {
			this->n = n;
			//this->r = r;
			this->pos = TVektor(x,y);
			this->next = TVektor(x,y);
			this->v = TVektor(0.0,0.0);
			this->m = 100.0;
			this->r = this->m/3;
			this->inGame = true;
			this->color = color;
		}

		void move(int mode) {
			if (norm(this->next) < 0.001 ) {
				this->pos = this->next;
				this->next = TVektor(0.0,0.0);
			}
			this->pos += this->v;
			switch(mode) {
				case 0:
					this->v -= 2 * my * eye(this->v);
					break;
				case 1:
					this->v *= 1 - my;
					break;
				case 2:
					this->v = min(this->v - (2 * my * eye(this->v)), this->v * (1 - my));
					break;
			}
			if(norm(this->v) < 0.) this->v = TVektor(0.0,0.0);
		}

		void setMass(double m){
			this->m = m;
			this->r = this->m/3;
			if(! this->n) Stockradius = this->r;
		}
};


class Billard : public TPlan {
	private:
    		double y,z,	// Koordinatesystem
		       xZ,yZ,	// Queue Ziel
		       force,	// Kraft beim Stoss mit dem Queue
                       t1,t2,
                       K0w,K0h;
                int    Mode, K0On;

		TVektor Z,K0;
		bool moving, K0Move;	// Bewegen sich Kugeln?

                Kugel kugeln[N];
		Loch loecher[6];
		//double kugeln_X[N];
		//double kugeln_Y[N];
		//TColor colors[N];

	public:
	void Init();
	void Run();
	void Reset();
        void RunTaste0();
        void RunTaste1();
        void RunTaste2();
        void RunTaste3();

        void RunTaste6();
        void RunTaste7();
        void RunTaste8();
        void RunTaste10();
        void RunTaste11();

        void RunTaste14();
        void RunTaste15();
        void RunTaste16();
        void RunTaste17();

	void SetProductionError(int p);

	void Stoss();
	bool CheckMoving();
	void HandleHole();
	void HandleBoxCollision();
	void HandleBallCollision();
	void ElasticCollision(Kugel& k1,Kugel& k2, double dist);
	void CheckHoles();
        void CheckFouls();
        void CheckWinner();
	void DrawKugeln();
	void DrawTable();
        void DrawInfo();
        void DrawFoul();
        void Drawgamewin();
        void Drawgameover();
	void Debug() {
	}

	void BildMouseDown(int x, int y){
                Foul = false;
                FoulK0 = false;
                Treffer = false;
                erststoss = false;
                insloch = false;
		if ( ! moving && ! Foul && ! K0Move) Stoss();
	}

	void BildMouseMove(int x, int y, int left){
                        Z[0] = IntToX(x);
		        Z[1] = IntToY(y);
                if(FoulK0 && ! moving){
                        kugeln[0].pos[0]=IntToX(x);
                        kugeln[0].pos[1]=IntToY(y);
                        if(kugeln[0].pos[0]-kugeln[0].r < leftBorder) kugeln[0].pos[0]=leftBorder+kugeln[0].r;
                        if(kugeln[0].pos[0] > -635.0) kugeln[0].pos[0]=-635.0;
                        if(kugeln[0].pos[1]-kugeln[0].r < bottomBorder) kugeln[0].pos[1]=-600.0+kugeln[0].r;
                        if(kugeln[0].pos[1]+kugeln[0].r > topBorder) kugeln[0].pos[1]=600.0-kugeln[0].r;
                        K0Move = true;
                        }
	}

        void BildMouseUp(int x, int y){
            //if ( dist(kugeln[0].pos[0], kugeln[0].pos[1], IntToX(x), IntToY(y))<r) K0Move=0;
            if ( dist(0.0, 0.0, IntToX(x), IntToY(y))<1230.0) K0Move=false;
            }

	void InitQueue(){
	    SetPen(Hellrot,5);
	    SetBrush(Hellrot);
	}

	void DrawForce(double distance){
		if(Mode==0) force = (distance*0.9 < 800) ?  distance*0.9 : 800;
                // if(Mode==1) force = ((800-(t2-t1))> 10)  ?  (800-(t2-t1)): 10;
                if(Mode==1){
                   if((t2-t1) <= 790)   force = 800-(t2-t1);
                   if((t2-t1) >= 790 && (t2-t1)<=790*2)  force = -780+(t2-t1);
                   if((t2-t1)>=790*2 && (t2-t1) <=790*3)  force = 2380-(t2-t1);
                   if((t2-t1)>=790*3 && (t2-t1)<=790*4)  force = -2360+(t2-t1);
                   if((t2-t1)>=790*4) force = 10;
                   }
		SetPen(Rot);
		SetBrush(Rot);
		Circle(-400.0,-815.0,30.0);
		Rectangle(-400.0,-845.0,force,60.0);
		Circle(400.0,-815.0,30.0);
	}

	void DrawQueue(int show=1){
	    SetPen(Hellrot,5);
	    SetBrush(Hellrot);
	    //Circle(xK,yK,Stockradius);

	    SetPen(Schwarz,3);
	    MoveTo(kugeln[0].pos[0],kugeln[0].pos[1]);
	    LineTo(Z[0],Z[1]);
	    SetBrush(Klar);
	    Circle(Z[0],Z[1],Stockradius);
	    double distance = dist(kugeln[0].pos[0], kugeln[0].pos[1], Z[0], Z[1]);
	    if(distance==0) distance =  Stockradius;
	    DrawForce(distance);
	    double phi = acos((kugeln[0].pos[0]-Z[0])/distance);
	    if(Z[1]>kugeln[0].pos[1]) phi = -phi;
	    SetPen(Blau,15);
	    MoveTo(kugeln[0].pos[0]+60.0*cos(phi),kugeln[0].pos[1]+60.0*sin(phi));
	    LineTo(kugeln[0].pos[0]+Stocklang*cos(phi),kugeln[0].pos[1]+Stocklang*sin(phi));}
};

double distKugeln(Kugel k1, Kugel k2) {
	return sqrt(pow(k1.pos[0] - k2.pos[0],2) + pow(k1.pos[1] - k2.pos[1],2));
}

bool insLoch(Kugel k, Loch l) {
	return sqrt(l.r * l.r) >= sqrt(pow(k.pos[0] - l.pos[0],2) + pow(k.pos[1] - l.pos[1],2));
}

void Billard::Init() {
	ProgrammName = "Billard";

	CallRunTime = 3;
	moving = false;

	Scale(-1700.0,1700.0,0.0);
	y = IntToY(0);
	Scale(-1700.0,1700.0,-y/2.0);

        InsertTaste(0,"Distanzenkraft");
        InsertTaste(1,"Zeitenkraft");
        InsertTaste(2,"Spieler1_Name");
        InsertTaste(3,"Spieler2_Name");

        InsertTaste(6,"Linear");
        InsertTaste(7,"Exponentiel");
        InsertTaste(8,"Gemischt");
        InsertTaste(10,"-my");
        InsertTaste(11,"+my");

        InsertTaste(14,"Masse -10%");
        InsertTaste(15,"Masse +10%");

        InsertTaste(16,"Genaugikeit -1%");
        InsertTaste(17,"Genauigkeit +1%");
	Reset();
}


void Billard::Run() {
	moving = CheckMoving();
	if ( moving ) {
                Reihe = true; //das bedeutet, diese Runde laeuft
                t1=t2;
		HandleBoxCollision();
		HandleBallCollision();
                CheckHoles();
		for(int i = 0; i < N; i++) {
			kugeln[i].move(FrictionMode);
		}
	}
	if(! moving && Reihe){
		//if(! Treffer) Foul = true;
    		CheckFouls();
    		if(Foul || ! insloch){
    			Spieler1 = ! Spieler1;
    			Spieler2 = ! Spieler2;
    		}
    		Reihe = false; 
	} // wenn keiner Kugeln ins Loch geht, oder wenn es ein Foul eintritt, wird der Spieler gewechselt

    	if(counter%2 == 0) {
    		DrawTable();
    		DrawKugeln();
    	        DrawInfo();
    		if ( ! moving && ! K0Move) DrawQueue();
    		//if ( ! moving && ! K0Move) DrawQueue();
    	}

	CheckWinner();
	Drawgamewin();
	DrawFoul();
	Drawgameover();

	CallRun = True;
	counter++;
	t2=counter;
}

void Billard::Reset(){
        Clear(Schwarz);
	DrawTable();
	InitQueue();

        ChangeName=1;
        DrawInfo();

        for(int i = 0; i < N; i++) {
		kugeln[i].init(i, kugeln_X[i], kugeln_Y[i], colors[i]);
	}
        for(int i = 0; i < 6; i++) {
		loecher[i].init(i, loecher_X[i], loecher_Y[i]);
	}
	DrawKugeln();

        Z = TVektor(0.0,0.0);
        Mode = 0;
        //K0On = 0;
        K0Move = false;
	Foul = false;        
	FoulK0 = false;
	Treffer = false;     
	insloch = false;    
	Spieler1 = true;    
	Spieler2 = false;   
	Reihe = false;      
	erststoss = false;  
	Ordnung = 0;        
	Gameon = false;     
	gameover=false;
	gamewin=false;
	counter = 0;
	ProductionError = 0;
}


void Billard::RunTaste0(){ Mode = 0;}

void Billard::RunTaste1(){ Mode = 1;}

void Billard::RunTaste2(){
	Name1Input = InputBox("Hello","Bitte geben Sie Ihren Namen ein:","input");
	ChangeName=2;
	DrawInfo();
}

void Billard::RunTaste3(){
	Name2Input = InputBox("Hello","Bitte geben Sie Ihren Namen ein:","input");
	ChangeName=3;
	DrawInfo();
}

void Billard::RunTaste6(){FrictionMode = 0;}
void Billard::RunTaste7(){FrictionMode = 1;}
void Billard::RunTaste8(){FrictionMode = 2;}
void Billard::RunTaste10(){my *= 0.9;}
void Billard::RunTaste11(){my *= 1.1;}

void Billard::RunTaste14(){
	if(kugeln[0].m * 0.9 > 50){
		kugeln[0].setMass(kugeln[0].m * 0.9);
	}
}
void Billard::RunTaste15(){
	if(kugeln[0].m * 1.1 < 250){
		kugeln[0].setMass(kugeln[0].m * 1.1);
	}
}

void Billard::RunTaste16(){
	ProductionError = (ProductionError - 1 >= 0) ? ProductionError -= 1 : 0;
	SetProductionError(ProductionError);
}

void Billard::RunTaste17(){
	ProductionError += 1;
	SetProductionError(ProductionError);
}

void Billard::SetProductionError(int p){
	for(int i = 1; i < N; i++){
		kugeln[i].setMass(randInt(100-p,100+p));
	}
}

void Billard::DrawTable(){
	Clear();
	SetPen(Schwarz);
	z=RGBSkala(50.0,30.0,0.0);
	SetBrush(z);
	Rectangle(-1405.0,-765.0,2810.0,1530.0);
	Rectangle(-1405.0,-865.0,2810.0,100.0);

        Rectangle(-1405.0,765.0,2810.0,100.0);

	SetPen(Schwarz,2);
	SetBrush(Gruen);

	//Rectangle(-1270.0,-635.0,2540.0,1270.0); // Spielfeld (ohne Bande)
        Rectangle(-1250.0,-615.0,2500.0,1230.0); // Spielfeld (ohne Bande)

	SetPen(Grau);
	SetBrush(Grau);
	Rectangle(-400.0,-845.0,800.0,60.0);
	Circle(-400.0,-815.0,30.0);
	Circle(400.0,-815.0,30.0);
	SetPen(Schwarz,1);
	SetBrush(Schwarz);
        Circle(-1210.0,-575.0,60.0);// L?cher
        Circle(-1210.0,575.0,60.0);
        Circle(1210.0,-575.0,60.0);
        Circle(1210.0,575.0,60.0);
        Circle(0.0,-600.0,60.0);
        Circle(0.0,600.0,60.0);// L?cher Ende  */

	SetPen(Schwarz,2);
	MoveTo(-1220.0,-635.0); // Bande
	LineTo(-1180.0,-595.0);
        LineTo(-50.0,-595.0);
	MoveTo(1220.0,-635.0);
	LineTo(1180.0,-595.0);
        LineTo(50.0,-595.0);
	MoveTo(1220.0,635.0);
	LineTo(1180.0,595.0);
        LineTo(50.0,595.0);
	MoveTo(-1220.0,635.0);
	LineTo(-1180.0,595.0);
        LineTo(-50.0,595.0);
	MoveTo(-1270.0,-585.0);
	LineTo(-1230.0,-545.0);
	LineTo(-1230.0,545.0);
	MoveTo(1270.0,-585.0);
	LineTo(1230.0,-545.0);
	LineTo(1230.0,545.0);
	SetPen(Schwarz,1);
	MoveTo(-635.0,595.0);
	LineTo(-635.0,-595.0);
	SetPen(Weiss);
	SetBrush(Weiss);
        Circle(-300.0,680.0,10.0);
	Circle(-600.0,680.0,10.0);
	Circle(-900.0,680.0,10.0);
	Circle(300.0,680.0,10.0);
	Circle(600.0,680.0,10.0);
	Circle(900.0,680.0,10.0);
	Circle(-300.0,-680.0,10.0);
	Circle(-600.0,-680.0,10.0);
	Circle(-900.0,-680.0,10.0);
	Circle(300.0,-680.0,10.0);
	Circle(600.0,-680.0,10.0);
	Circle(900.0,-680.0,10.0);
	Circle(-1320.0,0.0,10.0);
	Circle(-1320.0,350.0,10.0);
	Circle(-1320.0,-350.0,10.0);
	Circle(1320.0,0.0,10.0);
	Circle(1320.0,350.0,10.0);
	Circle(1320.0,-350.0,10.0);
}
void Billard::DrawInfo(){
        SetPen(Schwarz,3);
        SetTextSize(20);
        SetBrush(Klar);
        Text(-500.0,925.0,"Spieler1 : ");
        Text(350.0,925.0,"Spieler2 : ");
        if(ChangeName==1){ Name1Text="Jack";
        Text(-390.0,925.0,Name1Text);
        Name2Text="Tom";
        Text(460.0,925.0,Name2Text);}
        else if (ChangeName==2){
        SetBrush(Weiss);
        Name1Text=PlanString(Name1Input.c_str());
        Text(-390.0,925.0,Name1Text);
        Text(460.0,925.0,Name2Text);}
        else if (ChangeName==3){
        SetBrush(Weiss);
        Text(-390.0,925.0,Name1Text);
        Name2Text=PlanString(Name2Input.c_str());
        Text(460.0,925.0,Name2Text);}


        SetPen(Schwarz);
        SetBrush(Schwarz);
        Rectangle(-300.0,880.0,600.0,50.0);
        SetBrush(Gelb);
        if(Spieler1) {Circle(-280.0,905.0,20.0); }//Spieler11=Spieler1;}
        if(Spieler2) {Circle(280.0,905.0,20.0);  }//Spieler22=Spieler2;}
        // gleber Kugel bedeutet, welcher Spieler jetzt in der Runde ist
        if(Ordnung == 1) {
               Text(-520.0,980.0,"Vollfarbe Kugeln");
               Text(320.0,980.0,"Halbfarbe Kugeln");
	}
	else if(Ordnung == 2) {
               Text(-520.0,980.0,"Halbfarbe Kugeln");
               Text(320.0,980.0,"Vollfarbe Kugeln");
	}

        SetPen(Schwarz);
        SetBrush(Weiss);
	PlanString reibung;
	switch(FrictionMode){
		case 0: 
			reibung = "Linear";
			break;
		case 1: 
			reibung = "Exponential";
			break;
		case 2: 
			reibung = "Gemischt";
			break;
	}
	Text(1450.0, 300.0, "Reibung:");
	Text(1600.0, 300.0, reibung);
	Text(1450.0, 250.0, "My:");
	Text(1600.0, 250.0, my);

	Text(1450.0, 150.0, "Spielball m:");
	Text(1600.0, 150.0, kugeln[0].m);
	Text(1450.0, 100.0, "Produktionsfehler:");
	Text(1600.0, 100.0, ProductionError);
	Text(1620.0, 100.0, "%");

}


void Billard::DrawKugeln() {
	double x, y, r;
	int n;
	TColor color; 
	PlanString label;
	for(int i = 0; i < N; i++) {
		n = kugeln[i].n;
		r = kugeln[i].r;
		x = kugeln[i].pos[0];
		y = kugeln[i].pos[1];
		label = kugeln[i].n;
		color = kugeln[i].color;
		if( ! n ) {
			SetPen(Schwarz, 2);
			SetBrushColor(color);
			Circle(x,y,r);
		}
		else if( 8 < n ) {
			SetPen(Weiss);
			SetBrush(Weiss);
			Circle(x,y,r);
			SetPenColor(color);
			SetBrushColor(color);
			View(x-r, y-r*0.7, 2*r, 1.4*r);
			Scale(x-r, x+r, y - r*0.7, y+0.7*r);
			Circle(x,y,r);
                        SetTextSize(15);
			Text(x-0.5*r,y+0.5*r,label);

			View();
			Scale(-1700.0,1700.0,0.0);
			y = IntToY(0);
			Scale(-1700.0,1700.0,-y/2.0);
			SetPen(Schwarz,2);
			SetBrush(Klar);
			Circle(x,y,r);
		}
		else {
			SetPen(Schwarz, 2);
			SetBrushColor(color);
			Circle(x,y,r);
                        SetTextSize(15);
			Text(x-0.5*r,y+0.5*r,label);
		}
	}
}

void Billard::Stoss() {
	moving = true;
	kugeln[0].v = ForceScale * force * ( Z - kugeln[0].pos)/norm( Z - kugeln[0].pos);
}

bool Billard::CheckMoving() {
	bool m = false;
	for(int i = 0; i < N; i++) {
		if ( MaxNorm(kugeln[i].v) > 0.1) m = true;
	}
	return m;
}

void Billard::CheckHoles() {
        for(int i = 0; i < N; i++) {
        	for(int j = 0; j < 6; j++) {
			if(insLoch(kugeln[i],loecher[j])){
               			if(Ordnung == 1)    kugeln[i].pos = TVektor(i*100.0-800.0,800.0);
               			if(Ordnung == 2){   if(i<8)  kugeln[i].pos = TVektor(i*100.0,800.0);
                                   	if(i==8) kugeln[i].pos = TVektor(i*100.0-800.0,800.0);
                                   	if(i>8)  kugeln[i].pos = TVektor((i-8)*100.0-800.0,800.0);
				}
                        	kugeln[i].v = TVektor(0.0,0.0);
                        	kugeln[i].inGame = false;
                        	insloch = true;

				if(! Ordnung && i !=8 && i) {
					if(Spieler1) {
                        			if(i < 8) Ordnung=1;
						else Ordnung=2;
					}
					else {
                        			if(i > 8) Ordnung=1;
						else Ordnung=2;
					}
				}
			}
		}
	}
}



void Billard::CheckFouls(){
        if(! kugeln[0].inGame) {
               FoulK0 = true;
               Foul = true;
               kugeln[0].inGame = true;
	}
	for(int i = 1; i < N; i++) {
                if(! kugeln[i].inGame){
			if(Ordnung == 1) {
				if(Spieler1) if(i < 8) continue;
				else if(i > 8) continue;
			} 
			else if(Ordnung == 2){
				if(Spieler1) if(i > 8) continue;
				else if(i < 8) continue;
			}
			Foul = true;
			//kugeln[i].inGame = false;
                }
        }
}


void Billard::DrawFoul(){
     if(moving && Foul ){
     SetPen(Schwarz,3);
     SetTextSize(20);
     SetBrush(Gelb);
     SetInfo("Du hast ein Foul!");
     Text(-90.0,920.0,"Spieler austauschen!");
     if(t2-t1>50) Foul = false;
     }
     // wenn ein Foul eintritt, dann tauscht ein Satz auf
}


void Billard::Drawgameover(){
        if(gameover ){
        MessageBox(NULL,"Spiel Rest!","Gameover!",MB_OK);
        Billard::Reset();
        gameover = false;  }

}

void Billard::CheckWinner(){
     if(! kugeln[8].inGame) {
         if(!Ordnung) gameover = true;
         if(Ordnung == 1 && Spieler1) {
            if( ! kugeln[1].inGame && ! kugeln[2].inGame && ! kugeln[3].inGame && ! kugeln[4].inGame
                && ! kugeln[5].inGame && ! kugeln[6].inGame && ! kugeln[7].inGame) {
                gamewin = true;
	    }
             else gameover = true;
                   }

         if(Ordnung == 1 && Spieler2){
            if( !kugeln[9].inGame&&!kugeln[10].inGame&&!kugeln[11].inGame&&!kugeln[12].inGame
                &&!kugeln[13].inGame&&!kugeln[14].inGame&&!kugeln[15].inGame){
                 gamewin = true;}
            else gameover = true;
                }

         if(Ordnung == 2 && Spieler2 ){
            if(!kugeln[1].inGame &&!kugeln[2].inGame&&!kugeln[3].inGame&&!kugeln[4].inGame
               &&!kugeln[5].inGame&&!kugeln[6].inGame&&!kugeln[7].inGame) {
               gamewin = true;}
            else gameover = true;

                }

         if(Ordnung == 2 && Spieler1){
		if( !kugeln[9].inGame&&!kugeln[10].inGame&&!kugeln[11].inGame&&!kugeln[12].inGame
            &&!kugeln[13].inGame&&!kugeln[14].inGame&&!kugeln[15].inGame){
            gamewin = true;}
          else gameover = true;
		}
	} 
}

void Billard::Drawgamewin(){
     if(gamewin){
     MessageBox(NULL,"Du bist Gewinner!","Gameover!",MB_OK);
     Billard::Reset();
     gamewin = false;  }
     }

void Billard::HandleBoxCollision() {
	double tcx;	// Zeitanteil in dem die Kollision stattfinden w��1rde in x
	double tcy;	// Zeitanteil in dem die Kollision stattfinden w��1rde in y
	double r;
	for(int i = 0; i < N; i++) {
		r = kugeln[i].r;
		kugeln[i].next = kugeln[i].pos + kugeln[i].v * (1 - my);

		//CheckHoles(i);
                if(kugeln[i].next[1]<650){
		if ( kugeln[i].next[0] - kugeln[i].r < leftBorder ) {
			tcx = (leftBorder + kugeln[i].r - kugeln[i].next[0])/(kugeln[i].pos[0] - kugeln[i].next[0]);
			kugeln[i].v[0] *= - 1;
			kugeln[i].next[0] = leftBorder + kugeln[i].r + kugeln[i].v[0] * (1 - my) * (1-tcx);
		}
		else if ( kugeln[i].next[0] + kugeln[i].r > rightBorder ) {
			tcx = ( rightBorder - kugeln[i].r - kugeln[i].next[0])/(kugeln[i].pos[0] - kugeln[i].next[0]);
			kugeln[i].v[0] *= - 1;
			kugeln[i].next[0] = rightBorder - kugeln[i].r + kugeln[i].v[0] * (1 - my) * (1-tcx);
		}
		if ( kugeln[i].next[1] - kugeln[i].r <  bottomBorder ) {
			tcy = ( bottomBorder + kugeln[i].r - kugeln[i].next[1])/(kugeln[i].pos[1] - kugeln[i].next[1]);
			kugeln[i].v[1] *= - 1;
			kugeln[i].next[1] = bottomBorder + kugeln[i].r + kugeln[i].v[1] * (1 - my) * (1-tcy);
		}
		else if ( kugeln[i].next[1] + kugeln[i].r > topBorder) {
			tcy = ( topBorder - kugeln[i].r - kugeln[i].next[1])/(kugeln[i].pos[1] - kugeln[i].next[1]);
			kugeln[i].v[1] *= - 1;
			kugeln[i].next[1] = topBorder - kugeln[i].r + kugeln[i].v[1] * (1 - my) * (1-tcy);
		}}
                else kugeln[i].v = TVektor(0.0,0.0);
	}
}


void Billard::ElasticCollision(Kugel& k1, Kugel& k2, double dist){
	TVektor vn;
	if (k1.m == 0. && k2.m == 0.) return;
	vn = (k2.pos - k1.pos)/norm(k2.pos - k1.pos);

	k1.pos -= (k1.r+k2.r - dist) * vn;
	k2.pos += (k1.r+k2.r - dist) * vn;

	double MassRatio = k1.m / k2.m;

	double delta = dot(vn, k2.v) - dot(vn, k1.v);

	k1.v += delta / MassRatio * vn ;
	k2.v -= delta * MassRatio * vn ;
}

void Billard::HandleBallCollision() {
	double delta, dist;
	for(int i = 0; i < N; i++) {
		for(int j = i+1; j < N; j++) {
			dist = distKugeln(kugeln[i],kugeln[j]);

			if(kugeln[i].r + kugeln[j].r > dist) {
                                Treffer = true;
                                if((i == 0) && ! erststoss){
					erststoss = true;
					if(j==8) Foul = true;
					if(Ordnung == 1) {
						if(Spieler1){
							if(j > 8) Foul = true;
						}
						else{
							if(j < 8) Foul = true;
						}
					}
					else if(Ordnung == 2) {
						if(Spieler1){
							if(j < 8) Foul = true;
						}
						else{
							if(j > 8) Foul = true;
						}
					} // wenn man in jeder Runde beim erstem Stoss falsen Kugel gestosst, tritt ein Foul ein
				}
				ElasticCollision(kugeln[i],kugeln[j],dist);
			}
		}
	}
}

#pragma argsused
int main(int argc, char* argv[]) {
	Billard billard;
	billard.Execute();
	return 0;
}
