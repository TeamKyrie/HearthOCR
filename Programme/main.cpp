
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <opencv/cv.h>
#include <opencv/highgui.h>
#include <tesseract/baseapi.h>
#include <leptonica/allheaders.h>
#include <iostream>   // std::cout
#include <string>
#include <bsoncxx/builder/stream/document.hpp>
#include <bsoncxx/json.hpp>
#include <mongocxx/client.hpp>
#include <mongocxx/stdx.hpp>
#include <mongocxx/uri.hpp>
#include <mongocxx/instance.hpp>
#include <fstream>

using bsoncxx::builder::stream::close_array;
using bsoncxx::builder::stream::close_document;
using bsoncxx::builder::stream::document;
using bsoncxx::builder::stream::finalize;
using bsoncxx::builder::stream::open_array;
using bsoncxx::builder::stream::open_document;

using namespace std;
using namespace cv;


//renvoi la valeur absolue
template<class T> T abs(T nbr) {
    if(nbr >= 0) return nbr;
    else return (- nbr);
}

//class carteHerthstone
class CarteHS{
	private :
		string name;
		string type;
		int attaque;
		int defense;
		int cost;
		string gameID;
		string description;

	public :
		/*//constructeur
		CarteHS();
		CarteHS(string n,string t){
			this->name=n;
			this->description=t;

		}*/
		//accesseur pour le nom
		string getName(){return this->name;};
		void setName(string x){this->name=x;}
		//accesseur pour le type
		string getType(){return this->type;};
		void setType(string y){this->type=y;}
		//accesseur gameID
		string getGameID(){return this->gameID;};
		void setGameID(string y){this->gameID=y;}
		//accesseur cost
		int getCost(){return this->cost;};
		void setCost(int y){this->cost=y;}
		//accesseur attaque
		int getAttaque(){return this->attaque;};
		void setAttaque(int y){this->attaque=y;}
		//accesseur defense
		int getDefense(){return this->defense;};
		void setDefense(int y){this->defense=y;}
		//accesseur desc
		string getDesc(){return this->description;};
		void setDesc(string y){this->description=y;}
};


//class myBD
class MyBaseData{
	private :
		vector<CarteHS> listeCarte;

	public :

		//méthode implémantant l'algo de la distance de levenshtein pour comparer nos string
		unsigned int levenshtein_distance(const std::string& s1, const std::string& s2)
		{
			const std::size_t len1 = s1.size(), len2 = s2.size();
			std::vector<unsigned int> col(len2+1), prevCol(len2+1);

			for (unsigned int i = 0; i < prevCol.size(); i++)
				prevCol[i] = i;
			for (unsigned int i = 0; i < len1; i++) {
				col[0] = i+1;
				for (unsigned int j = 0; j < len2; j++)
		                        // note that std::min({arg1, arg2, arg3}) works only in C++11,
		                        // for C++98 use std::min(std::min(arg1, arg2), arg3)
					col[j+1] = std::min({ prevCol[1 + j] + 1, col[j] + 1, prevCol[j] + (s1[i]==s2[j] ? 0 : 1) });
				col.swap(prevCol);
			}
			return prevCol[len2];
		}

		void ajouterCarte(CarteHS x){
			this->listeCarte.push_back(x);
		}
		//void supprimerCarte(CarteHS x){this.listeCarte };
		CarteHS chercherCarte(string n){
			//std::cout<<"carte chercher "<<n<<"\n";
			unsigned int bestScore=1000;
			CarteHS meilleurVal;
			vector<CarteHS> lesMeilleursVal;
			for(auto it=this->listeCarte.begin() ; it < this->listeCarte.end(); it++){
				unsigned int result=levenshtein_distance(n,(*it).getName());
				if(result<=bestScore){
					if(result<bestScore){
						lesMeilleursVal.clear();
						lesMeilleursVal.push_back(*it);
						//std::cout<<(*it).getName()<<"\n";
						bestScore=result;
					}else{
						lesMeilleursVal.push_back(*it);
						bestScore=result;
					}
				}
			}


				//On parcourt le vector des meilleurs valeurs trouver avec levensthein, puis on compare sur la taille de la chaine de caractere
				unsigned int bestDelta=1000;
				for(auto it=lesMeilleursVal.begin() ; it < lesMeilleursVal.end(); it++){
					int delta=(n.size() - (*it).getName().size());
					delta=abs(delta);
					//std::cout<<"DELTA "<<delta;
					if(delta<bestDelta){
						meilleurVal=(*it);
						//std::cout<<"nom carte chercher :"<<(*it).getName();
					}

				}


			//std::cout<<"nom carte chercher :"<<meilleurVal.getName()<< " gameID :"<<meilleurVal.getGameID()<<"\n type: "<<meilleurVal.getType()<<" coût: "<<meilleurVal.getCost()<<" attaque: "<<meilleurVal.getAttaque()<<" defense: "<<meilleurVal.getDefense()<<"\n description :"<<meilleurVal.getDesc()<<"\n";
			return meilleurVal;

		}

    CarteHS chercherMain(string n, vector<CarteHS> mainAllie){
      unsigned int bestScore=1000;
			CarteHS meilleurVal;
			for(auto it=mainAllie.begin() ; it < mainAllie.end(); it++){
				unsigned int result=levenshtein_distance(n,(*it).getName());
				if(result<=bestScore){
					meilleurVal = (*it);
				}
			}
      return meilleurVal;
    }

		void afficheBaseData(){
			for(auto it=this->listeCarte.begin() ; it < this->listeCarte.end(); it++){
				std::cout<<"nom :"<<it->getName()<< " gameID :"<<it->getGameID()<<"\n type: "<<it->getType()<<" coût: "<<it->getCost()<<" attaque: "<<it->getAttaque()<<" defense: "<<it->getDefense()<<"\n description :"<<it->getDesc()<<"\n";
				}

		}

};

//méthode implémantant l'algo de la distance de levenshtein pour comparer nos string
unsigned int levenshtein_distance(const std::string& s1, const std::string& s2)
{
	const std::size_t len1 = s1.size(), len2 = s2.size();
	std::vector<unsigned int> col(len2+1), prevCol(len2+1);

	for (unsigned int i = 0; i < prevCol.size(); i++)
		prevCol[i] = i;
	for (unsigned int i = 0; i < len1; i++) {
		col[0] = i+1;
		for (unsigned int j = 0; j < len2; j++)

			col[j+1] = std::min({ prevCol[1 + j] + 1, col[j] + 1, prevCol[j] + (s1[i]==s2[j] ? 0 : 1) });
		col.swap(prevCol);
	}
	return prevCol[len2];
}

Pix *mat8ToPix(cv::Mat *mat8);
char* getOCR(Mat ref, tesseract::TessBaseAPI *myOCR);
char* getOCRboard(Mat ref, tesseract::TessBaseAPI *myOCR);
char* playOCR(Mat ref, tesseract::TessBaseAPI *myOCR);

vector<Point> contoursConvexHull( vector<vector<Point> > contours )
{
    vector<Point> result;
    vector<Point> pts;
    for ( size_t i = 0; i< contours.size(); i++)
        for ( size_t j = 0; j< contours[i].size(); j++)
            pts.push_back(contours[i][j]);
    convexHull( pts, result );
    return result;
}

int main(int argc,char **argv){

  ///////////////////////////////////////// Nom video
	char cNomVideo[250], cNomBD[250], cNomLog[250];

	if (argc != 4)
	{
		printf("Usage: Nom_Video Nom_BD Fichier_Log \n");
		return 1;
	}
	sscanf (argv[1],"%s",cNomVideo) ;
	sscanf (argv[2],"%s",cNomBD);
	sscanf (argv[3],"%s",cNomLog);

  mongocxx::instance instance{}; // Une SEUL instance mongo
	mongocxx::client client{mongocxx::uri{}};

	mongocxx::database db = client[cNomBD];

	mongocxx::collection coll = db["Cartes"];

	auto builder = bsoncxx::builder::stream::document{};

	//vector qui contiendra le nom de toutes les cartes de la collection
	vector<string> maListe;

	//object myBD qui contiendra notre base de donnée
	MyBaseData myBD;

	//on commence par crée les cartes heroPower qui ne sont pas dans la base de donnée
	CarteHS fireblast;
		fireblast.setName("Fireblast");
		fireblast.setDesc("Deal 1 damage");
		fireblast.setType("Hero Power");

	CarteHS steadyshot;
		steadyshot.setName("Steady Shot");
		steadyshot.setDesc("Deal 2 damage to hero ennemy");
		steadyshot.setType("Hero Power");

	CarteHS reinforce;
		reinforce.setName("Reinforce");
		reinforce.setDesc("Summon 1/1 silver hand");
		reinforce.setType("Hero Power");

	CarteHS totemiccall;
		totemiccall.setName("Totemic Call");
		totemiccall.setDesc("Summon a random Totem");
		totemiccall.setType("Hero Power");

	CarteHS daggerMistery;
		daggerMistery.setName("Dagger Mistery");
		daggerMistery.setDesc("Equip 1/2 dag");
		daggerMistery.setType("Hero Power");

	CarteHS lifeTap;
		lifeTap.setName("Life Tap");
		lifeTap.setDesc("-2 health, draw a card");
		lifeTap.setType("Hero Power");

	CarteHS shapeShift;
		shapeShift.setName("Shape Shift");
		shapeShift.setDesc("+1 attack this turn and +1 armor");
		shapeShift.setType("Hero Power");

	CarteHS armorup;
		armorup.setName("Armor Up");
		armorup.setDesc("+2 armor");
		armorup.setType("Hero Power");

	CarteHS laserHill;
		laserHill.setName("Lasser Hill");
		laserHill.setDesc("+2 health");
		laserHill.setType("Hero Power");

	//on ajoute ces heropower a notre myBD
	myBD.ajouterCarte(fireblast);
	myBD.ajouterCarte(steadyshot);
	myBD.ajouterCarte(reinforce);
	myBD.ajouterCarte(totemiccall);
	myBD.ajouterCarte(daggerMistery);
	myBD.ajouterCarte(lifeTap);
	myBD.ajouterCarte(shapeShift);
	myBD.ajouterCarte(armorup);
	myBD.ajouterCarte(laserHill);



/*	bsoncxx::document::value doc_value = builder
	  << "title" << " "
	  << "type" << " "
	  << bsoncxx::builder::stream::finalize; */

	//bsoncxx::document::view myview = doc_value.view();

	//find one document
	mongocxx::stdx::optional<bsoncxx::document::value> maybe_result =
	  coll.find_one(document{} << finalize);
	if(maybe_result) {
		//std::cout << bsoncxx::to_json(*maybe_result) << "\n";
	  // Do something with *maybe_result;
	}

	//find all documents // << "type" << "MINION"
	mongocxx::cursor cursorH = coll.find(document{} << "type" << "HERO" << finalize);

	for(auto doc : cursorH) {

			bsoncxx::document::element nCH{doc["name"]};
			bsoncxx::document::element tCH{doc["playerClass"]};
			bsoncxx::document::element gCH{doc["id"]};

			bsoncxx::document::element eCH{doc["health"]};

			std::string type = tCH.get_utf8().value.to_string();
			std::string name = nCH.get_utf8().value.to_string();
			std::string gameID = gCH.get_utf8().value.to_string();

			int defense = eCH.get_int32();


			CarteHS newCarte;
			newCarte.setName(name);
			newCarte.setType(type);
			newCarte.setGameID(gameID);
			newCarte.setDefense(defense);


			myBD.ajouterCarte(newCarte);
		    //bsoncxx::document::view myView = doc.view();
			//auto p=doc["title"];
	 	    //std::cout<<"nom: "<<name<<" type: "<<type<< "\n";
	 	    //maListe.push_back(name);

	}


	mongocxx::cursor cursor = coll.find(document{} << "type" << "MINION" << finalize);

	for(auto doc : cursor) {

			bsoncxx::document::element aC{doc["attack"]};
			bsoncxx::document::element cC{doc["cost"]};

			bsoncxx::document::element eC{doc["health"]};
			//bsoncxx::document::element gC{doc["gameId"]};
			bsoncxx::document::element nC{doc["name"]};
			bsoncxx::document::element tC{doc["type"]};



			//bsoncxx::document::element textC{doc["text"]};

			std::string type = tC.get_utf8().value.to_string();
			std::string name = nC.get_utf8().value.to_string();
			//std::string gameID = gC.get_utf8().value.to_string();

			int cost = cC.get_int32();
			int attack = aC.get_int32();
			int defense = eC.get_int32();

			//string desc=textC.get_utf8().value.to_string();

			CarteHS newCarte;

			newCarte.setName(name);
			newCarte.setType(type);
			//newCarte.setGameID(gameID);
			newCarte.setCost(cost);
			newCarte.setAttaque(attack);
			newCarte.setDefense(defense);
			//newCarte.setDesc(desc);

			myBD.ajouterCarte(newCarte);
		    //bsoncxx::document::view myView = doc.view();
			//auto p=doc["title"];
	 	    //std::cout<<"nom: "<<name<<" type: "<<type<< "\n";
	 	    //maListe.push_back(name);

	}

  std::vector<CarteHS> mainAllie;

  //On crée un flux et ouvre un fichier qui sert de log pour la partie analyser
  ofstream fichier(cNomLog, ios::out | ios::trunc);  //déclaration du flux et ouverture du fichier

          if(fichier)  // si l'ouverture a réussi
          {

	Mat hist_board_bas;
  Mat hist_board_bas_ref;

	Mat hist_board_haut;
	Mat hist_board_haut_ref;

	Mat hist_turn_ref;
	Mat hist_turn;

	Mat hist_vs;
	Mat hist_vs_ref;

	Mat test_bleue;
	Mat test_bleue_ref;

  Mat hist_fin_ref;
  Mat hist_fin;

  Mat hist_hero;
	Mat hist_hero_ref;

	Mat hist_hero_adverse;
	Mat hist_hero_adverse_ref;

	Mat hist_heropower;
	Mat hist_heropower_ref;

	Mat hist_heropower_adv;
	Mat hist_heropower_adv_ref;

	Mat hist_endturn;
	Mat hist_endturn_ref;

	Mat hist_coin1;
	Mat hist_coin1_ref;

	Mat hist_coin2;
	Mat hist_coin2_ref;

	Mat hist_pioche;
	Mat hist_pioche_ref;

	Mat hist_confirm_ref;
	Mat hist_confirm;

	Mat hist_card_adv_ref;
	Mat hist_card_adv;


	double txHaut_board = 0.95;
	double txBas_board = 0.6;

	double txVS = 0.5;

	double txTurn = 0.69;

	double txHeroPower = 0.7;
	double txEndTurn = 0.5; //0.5
	double txCoin = 0.7;


	/// Using 50 bins for hue and 60 for saturation
	int h_bins = 50; int s_bins = 60;
	int histSize[] = { h_bins, s_bins };

	// hue varies from 0 to 179, saturation from 0 to 255
	float h_ranges[] = { 0, 180 };
	float s_ranges[] = { 0, 256 };

	const float* ranges[] = { h_ranges, s_ranges };

	// Use the o-th and 1-st channels
	int channels[] = { 0, 1 };

	//test traitement carte pioché
	vector<Mat> channels0;

	VideoCapture cap(cNomVideo);
	bool playVideo = true;

  // On charge les images de réferences
	Mat board_ref= imread("Image_ref/board_bas.png");
	Mat board_haut_ref=imread("Image_ref/board_haut.png");
	Mat turn_ref=imread("Image_ref/turn.png");
	Mat vs_ref=imread("Image_ref/vs.png");
	Mat pioche_card_ref=imread("Image_ref/pioche_empty.png");
	Mat confirm_ref=imread("Image_ref/confirm.png");
	Mat card_ref=imread("Image_ref/card_adverse.png");
  Mat fin_ref=imread("Image_ref/fin.png");


	Mat heropower_ref=imread("Image_ref/heropower.png");
	Mat heropower_adv_ref=imread("Image_ref/heropower.png");
	Mat endturn_ref=imread("Image_ref/end_turn.png");
	Mat coin1_ref=imread("Image_ref/coin1.png");
	Mat coin2_ref=imread("Image_ref/coin2.png");

	// Histogramme des images de réferences
	calcHist( &board_ref, 1, channels, Mat(), hist_board_bas_ref, 2, histSize, ranges, true, false );
	normalize( hist_board_bas_ref, hist_board_bas_ref, 0, 1, NORM_MINMAX, -1, Mat() );

	calcHist( &board_haut_ref, 1, channels, Mat(), hist_board_haut_ref, 2, histSize, ranges, true, false );
	normalize( hist_board_haut_ref, hist_board_haut_ref, 0, 1, NORM_MINMAX, -1, Mat() );

	calcHist( &turn_ref, 1, channels, Mat(), hist_turn_ref, 2, histSize, ranges, true, false );
	normalize( hist_turn_ref, hist_turn_ref, 0, 1, NORM_MINMAX, -1, Mat() );

	calcHist( &vs_ref, 1, channels, Mat(), hist_vs_ref, 2, histSize, ranges, true, false );
	normalize( hist_vs_ref, hist_vs_ref, 0, 1, NORM_MINMAX, -1, Mat() );

	calcHist( &pioche_card_ref, 1, channels, Mat(), hist_pioche_ref, 2, histSize, ranges, true, false );
	normalize( hist_pioche_ref, hist_pioche_ref, 0, 1, NORM_MINMAX, -1, Mat() );

	calcHist( &confirm_ref, 1, channels, Mat(), hist_confirm_ref, 2, histSize, ranges, true, false );
	normalize( hist_confirm_ref, hist_confirm_ref, 0, 1, NORM_MINMAX, -1, Mat() );

	calcHist( &card_ref, 1, channels, Mat(), hist_card_adv_ref, 2, histSize, ranges, true, false );
	normalize( hist_card_adv_ref, hist_card_adv_ref, 0, 1, NORM_MINMAX, -1, Mat() );

	calcHist( &fin_ref, 1, channels, Mat(), hist_fin_ref, 2, histSize, ranges, true, false );
	normalize( hist_fin_ref, hist_fin_ref, 0, 1, NORM_MINMAX, -1, Mat() );


  calcHist( &heropower_ref, 1, channels, Mat(), hist_heropower_ref, 2, histSize, ranges, true, false );
  normalize( hist_heropower_ref, hist_heropower_ref, 0, 1, NORM_MINMAX, -1, Mat() );

  calcHist( &heropower_adv_ref, 1, channels, Mat(), hist_heropower_adv_ref, 2, histSize, ranges, true, false );
  normalize( hist_heropower_adv_ref, hist_heropower_adv_ref, 0, 1, NORM_MINMAX, -1, Mat() );

  calcHist( &endturn_ref, 1, channels, Mat(), hist_endturn_ref, 2, histSize, ranges, true, false );
  normalize( hist_endturn_ref, hist_endturn_ref, 0, 1, NORM_MINMAX, -1, Mat() );

  calcHist( &coin1_ref, 1, channels, Mat(), hist_coin1_ref, 2, histSize, ranges, true, false );
  normalize( hist_coin1_ref, hist_coin1_ref, 0, 1, NORM_MINMAX, -1, Mat() );

  calcHist( &coin2_ref, 1, channels, Mat(), hist_coin2_ref, 2, histSize, ranges, true, false );
  normalize( hist_coin2_ref, hist_coin2_ref, 0, 1, NORM_MINMAX, -1, Mat() );

  // On crée notre objet tesseract

	tesseract::TessBaseAPI *myOCR =
			new tesseract::TessBaseAPI();

	if (myOCR->Init(NULL, "eng")) {
		 fprintf(stderr, "Could not initialize tesseract.\n");
		 exit(1);
	 }

	int count_turn = 0;
	int i = 0;
	int i_adv = 0;
	int i_pioche = 0;
	int i_bas = 0;
  int i_haut = 0;
	bool test_turn = false;
	bool start = false;
	bool hero_found = false;
	bool test_confirm = false;
	bool fin_confirm = true;
	bool card_adv = false;
	bool t_pioche = false;
  bool invok = false;
  bool invok_haut = false;
  int cpt_invok_bas=0;
  int cpt_invok_haut=0;
	char* card1;
	char* card2;
	char* card3;
	char* card4;
	char* card1_first;
	char* card2_first;
	char* card3_first;
	char* card4_first;
	Mat M;

  int count_turn_adv = 0;
  //bool test_turn = false;
  bool test_turn_adv = false;
  int hpu = 0;
  int hpuAdv = 0;
  int cpt_hp=0;

  bool coin = false;
  bool test_coin = false;

  bool mull=false;

  bool impact_haut=false;
  bool impact_bas=false;
  bool combat_haut=false;
  bool combat_bas=false;

  CarteHS cardMulligan1_first;
  CarteHS cardMulligan2_first;
  CarteHS cardMulligan3_first;

	while(cap.isOpened()){
	//	printf("open");

		bool test = true;

		if(playVideo){

      // On récupère la frame dans une matrice
			test = cap.read(M);


			if(!test){
				printf("no");
				break;
			}

      // On découpe les régions d'intérets à partir de la frame

			Mat board_bas(M, Rect(M.cols*25/100,M.rows*58/100,M.cols*50/100,M.rows*3/100));
			Mat board_haut(M, Rect(M.cols*25/100, M.rows*40/100,M.cols*50/100,M.rows*3/100));
			Mat b_bas(M, Rect(M.cols*25/100,M.rows*52/100,M.cols*50/100,M.rows*6/100));
			Mat full_board_bas(M, Rect(M.cols*25/100,M.rows*47/100,M.cols*50/100,M.rows*16/100));
			Mat turn(M, Rect(M.cols*2/5,M.rows*3/7,M.cols*1/5,M.rows*1/10));
			Mat vs(M, Rect(M.cols*4/9,M.rows*3/8,M.cols*2/14,M.rows*3/12));
			Rect text1ROI(M.cols*62/100,M.rows*45/100,M.cols*20/100,M.rows*4/100);
			Mat hero_adverse(M, text1ROI);
			Rect text2ROI(M.cols*22.5/100,M.rows*93/100,M.cols*20/100,M.rows*4/100);
			Mat hero(M, text2ROI);
			Mat pioche_card(M, Rect(M.cols*69.5/100, M.rows*49/100, M.cols*15/100, M.rows*5/100));
			Mat card_adverse(M, Rect(M.cols*19.5/100, M.rows*39.5/100, M.cols*15/100, M.rows*6.5/100));
			Mat debCard_1(M, Rect(M.cols*27/100, M.rows*46/100, M.cols*11/100, M.rows*4/100));
			Mat debCard_2(M, Rect(M.cols*44.6/100, M.rows*46/100, M.cols*11/100, M.rows*4/100));
			Mat debCard_3(M, Rect(M.cols*62.5/100, M.rows*46/100, M.cols*11/100, M.rows*4/100));
			Mat confirm(M, Rect(M.cols*46.75/100, M.rows*77.5/100, M.cols*7/100, M.rows*4/100));
			Mat mana_main(M, Rect(M.cols*30/100,M.rows*85/100,M.cols*35/100,M.rows*10/100));
			Mat mana(M, Rect(M.cols*63.5/100, M.rows*91/100, M.cols*5/100, M.rows*3.5/100));
			Mat fin(M, Rect(M.cols*40/100,M.rows*55/100,M.cols*20/100,M.rows*10/100));
			Mat attaque_bas(M, Rect(M.cols*25/100,M.rows*50/100,M.cols*50/100,M.rows*7/100));
			Mat attaque_haut(M, Rect(M.cols*25/100, M.rows*35/100,M.cols*50/100,M.rows*7/100));
      Mat heropower(M, Rect(M.cols*7.8/14,M.rows*17.1/25,M.cols*1.55/20,M.rows*3.5/23));
			Mat heropower_adv(M, Rect(M.cols*7.7/14,M.rows*3.7/25,M.cols*1.6/20,M.rows*3.3/23));
			Mat endturn(M, Rect(M.cols*77.2/100,M.rows*42.5/100,M.cols*7.5/100,M.rows*7/100));
			Mat coin(M, Rect(M.cols*68/100,M.rows*47/100,M.cols*4/100,M.rows*7/100));


			if(!start){

        // ------------------- Récupération du "VS"
				calcHist( &vs, 1, channels, Mat(), hist_vs, 2, histSize, ranges, true, false );
				normalize( hist_vs, hist_vs, 0, 1, NORM_MINMAX, -1, Mat() );

				double res = compareHist( hist_vs_ref, hist_vs, 0 );
				if(res > txVS && res != 1){
					start = true;
					printf("début %f\n",res);
				}
			}

			if(start){

				if(!mull){
					if(!hero_found){

            // ------------------- Récupération des héros

						cvtColor( hero_adverse, hero_adverse, CV_BGR2GRAY );
						threshold( hero_adverse, hero_adverse, 200, 255,1 );
						Pix* ocrhero_adv = mat8ToPix(&hero_adverse);
						myOCR->SetImage(ocrhero_adv);
						char *text_hero_adv = myOCR->GetUTF8Text();
						delete [] ocrhero_adv;

						cvtColor( hero, hero, CV_BGR2GRAY );
						threshold( hero, hero, 200, 255,1 );
						Pix* ocrhero = mat8ToPix(&hero);
						myOCR->SetImage(ocrhero);
						char *text_hero = myOCR->GetUTF8Text();
						delete [] ocrhero;

						CarteHS hero1=myBD.chercherCarte(text_hero);
						CarteHS hero2=myBD.chercherCarte(text_hero_adv);

						fichier<<"Héros 1: "<<hero1.getName()<<", "<<hero1.getType()<<endl;
						fichier<<"Héros 2: "<<hero2.getName()<<", "<<hero2.getType()<<endl;

						printf("Adversaire : %s\nHeros : %s\n", text_hero_adv, text_hero);
						hero_found = true;

					}

					if(hero_found){

            //--------------------   Récupération de la pièce
            calcHist( &coin, 1, channels, Mat(), hist_coin1, 2, histSize, ranges, true, false );
      			normalize( hist_coin1, hist_coin1, 0, 1, NORM_MINMAX, -1, Mat() );

      			calcHist( &coin, 1, channels, Mat(), hist_coin2, 2, histSize, ranges, true, false );
      			normalize( hist_coin2, hist_coin2, 0, 1, NORM_MINMAX, -1, Mat() );

            double coin1 = compareHist( hist_coin1_ref, hist_coin1, 0 );
            double coin2 = compareHist( hist_coin2_ref, hist_coin2, 0 );


            if (coin1 > txCoin && !test_coin && coin1 < 1){
              test_coin = true;
              printf("\nHeros1 commence\n\n");
              fichier<<"Héros 1 commence"<<endl;
            }

            if (coin2 > txCoin && !test_coin && coin2 < 1){
              test_coin = true;
              printf("\nHéros 2  commence\n\n");
              fichier<<"Heros 2 commence"<<endl;
            }

            //--------------------- Partie analyse du mulligan

						calcHist( &confirm, 1, channels, Mat(), hist_confirm, 2, histSize, ranges, true, false );
						normalize( hist_confirm, hist_confirm, 0, 1, NORM_MINMAX, -1, Mat() );

						double res_conf = compareHist(hist_confirm_ref, hist_confirm, 0);

						if(res_conf > 0.8 && res_conf != 1 && !test_confirm){
							test_confirm = true;
							fin_confirm = false;
							printf("deb confirm %f\n", res_conf);

              card1_first = getOCR(debCard_1, myOCR);
              card2_first = getOCR(debCard_2, myOCR);
              card3_first = getOCR(debCard_3, myOCR);

              cardMulligan1_first=myBD.chercherCarte(card1_first);
              cardMulligan2_first=myBD.chercherCarte(card2_first);
              cardMulligan3_first=myBD.chercherCarte(card3_first);

							// imshow("conf", confirm);
						}

						if(res_conf < 0.5 && test_confirm && !fin_confirm){
							if(i==0)
								printf("fin confirm %f\n", res_conf);

							if(i>99){
								card1 = getOCR(debCard_1, myOCR);
								card2 = getOCR(debCard_2, myOCR);
								card3 = getOCR(debCard_3, myOCR);
							}

							fin_confirm = false;
							//test_confirm = false;
							if(!fin_confirm){
								i++;
								//printf("%d\n", i);
							}
							if(i>100){
								fin_confirm =true;
								i=0;

                CarteHS cardMulligan1=myBD.chercherCarte(card1);
                CarteHS cardMulligan2=myBD.chercherCarte(card2);
                CarteHS cardMulligan3=myBD.chercherCarte(card3);
                fichier<<"Mulligan : "<<endl;
                fichier<<"  Choix cartes : "<<cardMulligan1_first.getName()<<" / "<<cardMulligan2_first.getName()<<" / "<<cardMulligan3_first.getName()<<endl;
                fichier<<"  Cartes finales : "<<cardMulligan1.getName()<<" / "<<cardMulligan2.getName()<<" / "<<cardMulligan3.getName()<<endl;

                mainAllie.push_back(cardMulligan1);
                mainAllie.push_back(cardMulligan2);
                mainAllie.push_back(cardMulligan3);

								//printf("1 = %s, 2 = %s, 3 = %s\n", card1, card2, card3);
                mull=true;
							}
						}


					}

				}else{

          calcHist( &turn, 1, channels, Mat(), hist_turn, 2, histSize, ranges, true, false );
    			normalize( hist_turn, hist_turn, 0, 1, NORM_MINMAX, -1, Mat() );

          calcHist( &heropower, 1, channels, Mat(), hist_heropower, 2, histSize, ranges, true, false );
          normalize( hist_heropower, hist_heropower, 0, 1, NORM_MINMAX, -1, Mat() );

          calcHist( &heropower_adv, 1, channels, Mat(), hist_heropower_adv, 2, histSize, ranges, true, false );
          normalize( hist_heropower_adv, hist_heropower_adv, 0, 1, NORM_MINMAX, -1, Mat() );

          calcHist( &endturn, 1, channels, Mat(), hist_endturn, 2, histSize, ranges, true, false );
          normalize( hist_endturn, hist_endturn, 0, 1, NORM_MINMAX, -1, Mat() );

          double hp = compareHist( hist_heropower_ref, hist_heropower, 0 );
          double hpAdv = compareHist( hist_heropower_adv_ref, hist_heropower_adv, 0 );
          double res_turn = compareHist( hist_turn_ref, hist_turn, 0 );
          double res_endturn = compareHist( hist_endturn_ref, hist_endturn, 0 );

          // ------------------ Partie gestion des tours

          if(res_turn > txTurn && !test_turn){
            test_turn = true;
            test_turn_adv = false;
            count_turn++;
            if (count_turn_adv > 0){
              printf(" Fin Tour\n");
            }
            if (count_turn > count_turn_adv){
              fichier<<" "<<endl;
              fichier<<"Tour : "<<count_turn<<endl;
              fichier<<"    Heros 1 "<<endl;
              printf("Tour : %d\n", count_turn);
              printf("    Heros1 :\n");
            } else {
              printf("    Heros1 :\n");
              fichier<<"    Heros 1 "<<endl;
            }
            hpu = 0;
            cpt_hp=0;
            i=0;
          }

          if(res_endturn > 0.4 && !test_turn_adv && res_turn < txTurn){
            test_turn = false;
            test_turn_adv = true;
            count_turn_adv++;
            if (count_turn > 0){
              printf(" Fin Tour\n");
            }
            if (count_turn < count_turn_adv){
              fichier<<" "<<endl;
              fichier<<"Tour : "<<count_turn_adv<<endl;
              fichier<<"    Heros 2 "<<endl;
              printf("Tour : %d\n", count_turn_adv);
              printf("    Heros2 :\n");
            }
            else {
              printf("    Heros2 :\n");
              //fichier<<"Tour : "<<count_turn_adv<<endl;
              fichier<<"    Heros 2 "<<endl;
            }
            hpuAdv = 0;
            cpt_hp=0;
          }

          cpt_hp++;
          i++;

          if(count_turn > 0 || count_turn_adv > 0){

            // ----------------------- Partie gestion hero power

            if(count_turn > 0 && hp > txHeroPower && hp != 1 && hpu == 0 && !test_turn_adv && cpt_hp>60){
							hpu = 1;
							printf("  Joueur1 a utilisé le pouvoir héroïque\n");
              fichier<<"        Utilise le pouvoir héroïque"<<endl;
						}

						if(count_turn_adv > 0 && hpAdv > txHeroPower && hpAdv != 1 && hpuAdv == 0 && !test_turn && cpt_hp>60){
							hpuAdv = 1;
							printf("  Joueur2 a utilisé le pouvoir héroïque\n");
              fichier<<"        Utilise le pouvoir héroïque"<<endl;
						}


            // ------------------ Partie récupération de la carte piochée

  					calcHist( &pioche_card, 1, channels, Mat(), hist_pioche, 2, histSize, ranges, true, false );
  					normalize( hist_pioche, hist_pioche, 0, 1, NORM_MINMAX, -1, Mat() );

  					double p_res = compareHist( hist_pioche_ref, hist_pioche, 0 );
  					if(p_res < 0.103 && p_res > 0.09 && !t_pioche){
              i_pioche=0;
  						printf("pioche %f\n", p_res);
              CarteHS pioche=myBD.chercherCarte(getOCR(pioche_card, myOCR));
  						fichier<<"        Carte piochée: "<<pioche.getName()<<endl;
  						printf("%s\n", pioche.getName().c_str());
  						// imshow("card", pioche_card);
  						t_pioche=true;
              mainAllie.push_back(pioche);
  					}
  					if(t_pioche)
  						i_pioche++;

  					if(i_pioche > 100){
  						i_pioche=0;
  						t_pioche=false;
  					}

            // ------------------- Partie récupération de la carte jouée par l'adversaire

  					calcHist( &card_adverse, 1, channels, Mat(), hist_card_adv, 2, histSize, ranges, true, false );
  					normalize( hist_card_adv, hist_card_adv, 0, 1, NORM_MINMAX, -1, Mat() );

  					double ca_res = compareHist( hist_card_adv, hist_card_adv_ref, 0);
  					if(ca_res < 0.104 && ca_res > 0.08 && !card_adv){
              // imshow("card", card_adverse);
              i_adv=0;
              cpt_invok_haut=0;
  						printf("adverse %f\n", ca_res);

              CarteHS cardADV=myBD.chercherCarte(getOCR(card_adverse, myOCR));

              if(strcmp(cardADV.getType().c_str(),"Hero Power") != 0){
                fichier<<"        Joue: "<<cardADV.getName()<<endl;
                invok_haut = true;
              }
  						printf("%s\n", cardADV.getName().c_str());
  						card_adv = true;
  					}
  					if(card_adv){
  						i_adv++;
  					}
  					if(i_adv>125){
  						i_adv=0;
  						card_adv = false;

  					}
            if(invok_haut){
              cpt_invok_haut++;
            }

            // --------------- Partie gestion des attaques ---------------------

  					int cpt_blanc=0;
            int erosion_type = 2;
  					int erosion_size = 4;
  					Mat elem = getStructuringElement( erosion_type,
  																						 Size( 2*erosion_size + 1, 2*erosion_size+1 ),
  																						 Point( erosion_size, erosion_size ) );
          	inRange(attaque_haut, cv::Scalar(60, 180, 180), cv::Scalar(90, 255, 255), attaque_haut);
            dilate(attaque_haut, attaque_haut, elem);

            cpt_blanc=0;
            for( int i = 0; i < attaque_haut.rows; i++ )
              for( int j = 0; j < attaque_haut.cols; j++ )
              {
                if(attaque_haut.at<int>(i, j) != 0){
                  cpt_blanc++;
                }
              }
            if(cpt_blanc > 4300 && cpt_blanc < 4700 && !impact_haut){
              // printf("haut %d\n", cpt_blanc);
              // imshow("attaque_haut", attaque_haut);
              impact_haut=true;
              fichier<<"            Héros allié attaque héros adverse"<<endl;
              i_haut=0;
            }

            if(impact_haut)
              i_haut++;

            if(i_haut>150){
              impact_haut=false;
              i_haut=0;
            }

            if(i_haut==70)
              combat_haut=true;

            inRange(attaque_bas, cv::Scalar(60, 180, 180), cv::Scalar(90, 255, 255), attaque_bas);
            dilate(attaque_bas, attaque_bas, elem);
            cpt_blanc=0;
            for( int i = 0; i < attaque_bas.rows; i++ )
              for( int j = 0; j < attaque_bas.cols; j++ )
              {
                if(attaque_bas.at<int>(i, j) != 0){
                  cpt_blanc++;
                }
              }
            if(cpt_blanc > 4300 && cpt_blanc < 4700 && i > 100 && !impact_bas){
              // printf("bas %d\n", cpt_blanc);
              // imshow("attaque_bas", attaque_bas);
              fichier<<"            Héros adverse attaque héros allié"<<endl;
              impact_bas=true;
              i_bas=0;
            }

            if(impact_bas)
              i_bas++;

            if(i_bas > 150){
              impact_bas=false;
              i_bas=0;
            }

            if(i_bas==70)
              combat_bas=true;

            // ----------------------   Partie analyse des chiffre du board haut et bas

            // ----------------------   Partie board bas

            Mat mask_bas;

  					cvtColor(board_bas, mask_bas, CV_BGR2GRAY);
  					inRange(mask_bas, Scalar(30, 30, 30), Scalar(120, 120, 120), mask_bas);

  					dilate(mask_bas, mask_bas, elem);
  					dilate(mask_bas, mask_bas, elem);
  					dilate(mask_bas, mask_bas, elem);

            Mat dst_bas = Mat::zeros(board_bas.size(), board_bas.type());
            board_bas.copyTo(dst_bas, mask_bas);

            inRange(dst_bas, cv::Scalar(0, 0, 220), cv::Scalar(255, 255, 255), dst_bas);
  					erosion_size = 1;
  					Mat el = getStructuringElement( erosion_type,
  																						 Size( 2*erosion_size + 1, 2*erosion_size+1 ),
  																						 Point( erosion_size, erosion_size ) );
            erode(dst_bas, dst_bas, el);
            resize(dst_bas, dst_bas, Size(dst_bas.cols*2, dst_bas.rows*2));
            threshold( dst_bas, dst_bas, 100, 255,0 );
  					erosion_size = 0.5;
  					el = getStructuringElement( erosion_type,
  																						 Size( 2*erosion_size + 1, 2*erosion_size+1 ),
  																						 Point( erosion_size, erosion_size ) );
            erode(dst_bas, dst_bas, el);
            for(int i=0; i<4; i++){
              erode(dst_bas, dst_bas, el);
              dilate(dst_bas, dst_bas, el);
            }

            cpt_blanc=0;
            for( int i = 0; i < dst_bas.rows; i++ )
              for( int j = 0; j < dst_bas.cols; j++ )
              {
            		if(dst_bas.at<int>(i, j) == 255){
            			cpt_blanc++;
            		}
            	}
            if(cpt_blanc > 50 && cpt_blanc < 100 && cpt_invok_bas > 50){
              Pix* ocrpix = mat8ToPix(&dst_bas);
              myOCR->SetImage(ocrpix);
              char *chiffre_bas = myOCR->GetUTF8Text();
              //printf("%s\n", chiffre_bas);
              delete [] ocrpix;
              fichier << "            Carte invoquée par le héros allié état du board bas : "<<chiffre_bas<<endl;
              invok=false;
              cpt_invok_bas = 0;

            }

            // essaie de gestion d'affichage d'attaque

            /*if(cpt_blanc > 50 && cpt_blanc < 100 && combat_bas){
              combat_bas=false;
              Pix* ocrpix = mat8ToPix(&dst_bas);
              myOCR->SetImage(ocrpix);
              char *chiffre_bas = myOCR->GetUTF8Text();
              printf("combat bas %s\n", chiffre_bas);
              delete [] ocrpix;
              fichier << "          Héros adverse attaque héros allié état du board bas : "<<chiffre_bas<<endl;
            }*/

            // -------------------    Partie board haut

            Mat mask_haut;

  					cvtColor(board_haut, mask_haut, CV_BGR2GRAY);
  					inRange(mask_haut, Scalar(30, 30, 30), Scalar(120, 120, 120), mask_haut);

  					dilate(mask_haut, mask_haut, elem);
  					dilate(mask_haut, mask_haut, elem);
  					dilate(mask_haut, mask_haut, elem);

            Mat dst_haut = Mat::zeros(board_haut.size(), board_haut.type());
            board_haut.copyTo(dst_haut, mask_haut);
            // imshow("test", diist);
  					// blur( full_board_haut, full_board_haut, Size(3,3) );

            inRange(dst_haut, cv::Scalar(0, 0, 220), cv::Scalar(255, 255, 255), dst_haut);
  					erosion_size = 1;
  					el = getStructuringElement( erosion_type,
  																						 Size( 2*erosion_size + 1, 2*erosion_size+1 ),
  																						 Point( erosion_size, erosion_size ) );
            erode(dst_haut, dst_haut, el);
            resize(dst_haut, dst_haut, Size(dst_haut.cols*2, dst_haut.rows*2));
            threshold( dst_haut, dst_haut, 100, 255,0 );
  					erosion_size = 0.5;
  					el = getStructuringElement( erosion_type,
  																						 Size( 2*erosion_size + 1, 2*erosion_size+1 ),
  																						 Point( erosion_size, erosion_size ) );
            erode(dst_haut, dst_haut, el);
            for(int i=0; i<4; i++){
              erode(dst_haut, dst_haut, el);
              dilate(dst_haut, dst_haut, el);
            }

            cpt_blanc=0;
            for( int i = 0; i < dst_haut.rows; i++ )
              for( int j = 0; j < dst_haut.cols; j++ )
              {
            		if(dst_haut.at<int>(i, j) == 255){
            			cpt_blanc++;
            		}
            	}
            if(cpt_blanc > 50 && cpt_blanc < 100 && cpt_invok_haut > 50){
              Pix* ocrpix = mat8ToPix(&dst_haut);
              myOCR->SetImage(ocrpix);
              char *chiffre_haut = myOCR->GetUTF8Text();
              // printf("%s\n", chiffre_haut);
              delete [] ocrpix;
              fichier << "            Carte invoquée par le héros adverse état du board haut : "<<chiffre_haut<<endl;

              invok_haut=false;
              cpt_invok_haut=0;
            }

            // essaie de gestion d'affichage d'attaque

            /*if(cpt_blanc > 50 && cpt_blanc < 100 && combat_haut){
              combat_haut=false;
              Pix* ocrpix = mat8ToPix(&dst_haut);
              myOCR->SetImage(ocrpix);
              char *chiffre_haut = myOCR->GetUTF8Text();
              printf("combat haut %s\n", chiffre_haut);
              delete [] ocrpix;
              fichier << "          Héros allié attaque héros adverse état du board haut : "<<chiffre_haut<<endl;
            }*/




            // ------------------------  Partie carte invoquée par le héros allié

            erosion_type = 2;
            erosion_size = 10;
            Mat mask;
            Mat dstImage;
            inRange(b_bas, cv::Scalar(220, 50, 50), cv::Scalar(255, 190, 190), mask);
            elem = getStructuringElement( erosion_type,
                                              Size( 2*erosion_size + 1, 2*erosion_size+1 ),
                                              Point( erosion_size, erosion_size ) );
            for(int i=0; i<6; i++)
              dilate(mask, mask, elem);

            cpt_blanc=0;
            for( int i = 0; i < mask.rows; i++ )
             for( int j = 0; j < mask.cols; j++ )
             {
               if(mask.at<int>(i, j) != 0){
                 cpt_blanc++;
               }
             }

            if(cpt_blanc > 8000){
               dstImage = Mat::zeros(b_bas.size(), b_bas.type());
               Mat test;
               b_bas.copyTo(dstImage, mask);
               cvtColor( dstImage, test, CV_BGR2GRAY );
               threshold( test, test, 230, 255,0 );
               cpt_blanc=0;
               erosion_size = 0.7;
               Mat t = getStructuringElement( erosion_type,
                                                  Size( 2*erosion_size + 1, 2*erosion_size+1 ),
                                                  Point( erosion_size, erosion_size ) );
               for(int i=0; i<7; i++){
                 dilate(test, test, t);
                 erode(test, test, t);
               }

               for( int i = 0; i < test.rows; i++ )
                 for( int j = 0; j < test.cols; j++ )
                 {
                   if(test.at<int>(i, j) == 255){
                     cpt_blanc++;
                   }
                 }
               if(cpt_blanc > 380){

                 invok = true;

                 resize(dstImage, dstImage, Size(dstImage.cols*2, dstImage.rows*2));
                 inRange(dstImage, Scalar(200, 200, 200), Scalar(255, 255, 255), dstImage);
                 threshold( dstImage, dstImage, 220, 255, 1 );
                 erosion_size = 1;
                 Mat s = getStructuringElement( erosion_type,
                                                    Size( 2*erosion_size + 1, 2*erosion_size+1 ),
                                                    Point( erosion_size, erosion_size ) );

                 if(cpt_invok_bas==0){
                   Pix* ocrpix = mat8ToPix(&dstImage);
                   myOCR->SetImage(ocrpix);
                   char *text1 = myOCR->GetUTF8Text();
                   delete [] ocrpix;
                   CarteHS invoquee = myBD.chercherMain(text1, mainAllie);
                   fichier << "        Joue : "<<invoquee.getName()<<endl;
                   printf("%s\n", invoquee.getName().c_str());
                 }

               }
            }

            if(invok){
              cpt_invok_bas++;
            }


            // ----------------------- Découpage des régions du board

      /*		Mat haut_1_attaque(board_haut, Rect(board_haut.cols*1/100,0,board_haut.cols*3.2/100,board_haut.rows));
            Mat haut_1_vie(board_haut, Rect(board_haut.cols*9/100,0,board_haut.cols*3.2/100,board_haut.rows));
            Mat haut_2_attaque(board_haut, Rect(board_haut.cols*16/100,0,board_haut.cols*3.2/100,board_haut.rows));
            Mat haut_2_vie(board_haut, Rect(board_haut.cols*24/100,0,board_haut.cols*3.2/100,board_haut.rows));
            Mat haut_3_attaque(board_haut, Rect(board_haut.cols*30.5/100,0,board_haut.cols*3.2/100,board_haut.rows));
            Mat haut_3_vie(board_haut, Rect(board_haut.cols*38.5/100,0,board_haut.cols*3.2/100,board_haut.rows));
            Mat haut_4_attaque(board_haut, Rect(board_haut.cols*45/100,0,board_haut.cols*3.2/100,board_haut.rows));
            Mat haut_4_vie(board_haut, Rect(board_haut.cols*52.5/100,0,board_haut.cols*3.2/100,board_haut.rows));
            Mat haut_5_attaque(board_haut, Rect(board_haut.cols*60/100,0,board_haut.cols*3.2/100,board_haut.rows));
            Mat haut_5_vie(board_haut, Rect(board_haut.cols*67.5/100,0,board_haut.cols*3.2/100,board_haut.rows));
            Mat haut_6_attaque(board_haut, Rect(board_haut.cols*74/100,0,board_haut.cols*3.2/100,board_haut.rows));
            Mat haut_6_vie(board_haut, Rect(board_haut.cols*82/100,0,board_haut.cols*3.2/100,board_haut.rows));
            Mat haut_7_attaque(board_haut, Rect(board_haut.cols*89/100,0,board_haut.cols*3.2/100,board_haut.rows));
            Mat haut_7_vie(board_haut, Rect(board_haut.cols*96/100,0,board_haut.cols*3.2/100,board_haut.rows));*/

    /*			Mat bas_1_attaque(board_bas, Rect(board_bas.cols*1/100,0,board_bas.cols*3.2/100,board_bas.rows));
            Mat bas_1_vie(board_bas, Rect(board_bas.cols*9/100,0,board_bas.cols*3.2/100,board_bas.rows));
            Mat bas_2_attaque(board_bas, Rect(board_bas.cols*16/100,0,board_bas.cols*3.2/100,board_bas.rows));
            Mat bas_2_vie(board_bas, Rect(board_bas.cols*24/100,0,board_bas.cols*3.2/100,board_bas.rows));
            Mat bas_3_attaque(board_bas, Rect(board_bas.cols*30.5/100,0,board_bas.cols*3.2/100,board_bas.rows));
            Mat bas_3_vie(board_bas, Rect(board_bas.cols*38.5/100,0,board_bas.cols*3.2/100,board_bas.rows));
            Mat bas_4_attaque(board_bas, Rect(board_bas.cols*45/100,0,board_bas.cols*3.2/100,board_bas.rows));
            Mat bas_4_vie(board_bas, Rect(board_bas.cols*52.5/100,0,board_bas.cols*3.2/100,board_bas.rows));
            Mat bas_5_attaque(board_bas, Rect(board_bas.cols*60/100,0,board_bas.cols*3.2/100,board_bas.rows));
            Mat bas_5_vie(board_bas, Rect(board_bas.cols*67.5/100,0,board_bas.cols*3.2/100,board_bas.rows));
            Mat bas_6_attaque(board_bas, Rect(board_bas.cols*74/100,0,board_bas.cols*3.2/100,board_bas.rows));
            Mat bas_6_vie(board_bas, Rect(board_bas.cols*82/100,0,board_bas.cols*3.2/100,board_bas.rows));
            Mat bas_7_attaque(board_bas, Rect(board_bas.cols*89/100,0,board_bas.cols*3.2/100,board_bas.rows));
            Mat bas_7_vie(board_bas, Rect(board_bas.cols*96/100,0,board_bas.cols*3.2/100,board_bas.rows));*/


            // ---------------  Gestion de la fin de la partie
            calcHist( &fin, 1, channels, Mat(), hist_fin, 2, histSize, ranges, true, false );
            normalize( hist_fin, hist_fin, 0, 1, NORM_MINMAX, -1, Mat() );
            double res_fin = compareHist( hist_fin, hist_fin_ref, 0);
            if(res_fin > 0.8){
              char* t_fin = getOCR(fin, myOCR);
              printf("%s\n", t_fin);
              fichier<< "Fin de la partie : "<< t_fin <<endl;
              // imshow("fin", fin);
              mull=false;
              start=false;
            }


          }


				}

			}
			// imshow("1", debCard_1);
			// imshow("2", debCard_2);
			// imshow("3", debCard_3);

			// resize(board_bas, board_bas,Size(board_bas.cols*3, board_bas.rows*3));


			// cvtColor( confirm, confirm, CV_BGR2GRAY );
			// threshold( confirm, confirm, 200, 255,1 );
			// imshow("confirm", confirm);

			// imshow("hero", hero);
			// imshow("adverse", hero_adverse);
			// imshow("vide", card_adverse);
			// imshow("mana", mana);
			//
			// resize(M, M, Size(M.cols/2,M.rows/2));
			// imshow("MyVideo",M);

		}
// 		printf("res = %f\n",res);


		char key = cvWaitKey(1);
		if (key==27){
			printf("%c\n",key);
			break;
		}
		if(key=='p'){
			playVideo = !playVideo;
		}

	}
	// destroy tesseract OCR engine
	myOCR->Clear();
	myOCR->End();

  fichier.close();  // on referme le fichier
  }
  else  // sinon
          cerr << "Erreur à l'ouverture !" << endl;

	return 0;
}

Pix *mat8ToPix(cv::Mat *mat8)
{
    Pix *pixd = pixCreate(mat8->size().width, mat8->size().height, 8);
    for(int y=0; y<mat8->rows; y++) {
        for(int x=0; x<mat8->cols; x++) {
            pixSetPixel(pixd, x, y, (l_uint32) mat8->at<uchar>(y,x));
        }
    }
    return pixd;
}


char* getOCR(Mat ref, tesseract::TessBaseAPI *myOCR){

  // imshow("base", ref);
	int erosion_type = 2;
	int erosion_size = 1;
	resize(ref, ref, Size(ref.cols*3,ref.rows*3));
	cvtColor( ref, ref, CV_BGR2GRAY );
  // imshow("grey", ref);
	threshold( ref, ref, 230, 255,1 );
  // imshow("binaire", ref);
	Mat elem = getStructuringElement( erosion_type,
                                     Size( 2*erosion_size + 1, 2*erosion_size+1 ),
                                     Point( erosion_size, erosion_size ) );
	erode(ref, ref, elem);
	dilate(ref, ref, elem);
	// erode(ref, ref, elem);
	Pix* ocrpix = mat8ToPix(&ref);
	myOCR->SetImage(ocrpix);
	char *text1 = myOCR->GetUTF8Text();
	// printf("%s\n", text1);
	// imshow("card", ref);
	delete [] ocrpix;
	return text1;

}

char* playOCR(Mat ref, tesseract::TessBaseAPI *myOCR){
  int erosion_type = 2;
	int erosion_size = 1;
  threshold( ref, ref, 230, 255,1 );
	Mat elem = getStructuringElement( erosion_type,
                                     Size( 2*erosion_size + 1, 2*erosion_size+1 ),
                                     Point( erosion_size, erosion_size ) );
	erode(ref, ref, elem);
	dilate(ref, ref, elem);
	Pix* ocrpix = mat8ToPix(&ref);
	myOCR->SetImage(ocrpix);
	char *text1 = myOCR->GetUTF8Text();
	// printf("%s\n", text1);
	// imshow("card", ref);
	delete [] ocrpix;
	return text1;

}

char* getOCRboard(Mat ref, tesseract::TessBaseAPI *myOCR){

	int erosion_type = 2;
	int erosion_size = 1;
	// printf("board\n");
	resize(ref, ref, Size(ref.cols*2,ref.rows*2));
	cvtColor( ref, ref, CV_BGR2GRAY );
	threshold( ref, ref, 249, 255,1 );
	Mat elem = getStructuringElement( erosion_type,
                                     Size( 2*erosion_size + 1, 2*erosion_size+1 ),
                                     Point( erosion_size, erosion_size ) );
	erode(ref, ref, elem);
	erode(ref, ref, elem);
	// erode(ref, ref, elem);
	Pix* ocrpix = mat8ToPix(&ref);
	myOCR->SetImage(ocrpix);
	char *text1 = myOCR->GetUTF8Text();
	string s(text1);
	bool has_only_digits = (s.find_first_not_of( "0123456789" ) == string::npos);
	if(!has_only_digits){
		// printf("%s\n", text1);
		printf("%s\n", text1);
		// imshow("card", ref);
	}
	delete [] ocrpix;
	return text1;

}
