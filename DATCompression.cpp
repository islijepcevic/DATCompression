#include <iostream>
#include <vector>
#include <map>
#include <string>

using namespace std;

/*
 * string sam odabrao za radnu rijec jer ce biti lakse obradivati podatke
 * simboli u stringu ce biti charovi '1' ili '0'
 *
 * ako bude pre sporo, mozemo promijeniti algoritam na bitove
 */
typedef string tipRijec;

class DATCompression {

public:

    DATCompression() { }
    
    
    /*
     * ZADANA FUNKCIJA
     */
    vector< int > compress( vector< int > data );


private:
    
    //map (ili dict) : key - kodirani niz bitova, value - kodna rijec
    map< tipRijec, unsigned long long int > rjecnik;
    
   
    tipRijec radnaRijec; // tipRijec je string
    
    void initRjecnik() {

        rjecnik.clear();
        rjecnik[ "0" ] = 0;
        rjecnik[ "1" ] = 1;
        rjecnik[ "#" ] = 2;
    
    }
    
    
    char pronadjiBit( int integer, int pozicija ) {
        
        integer >>= pozicija;
        integer &= 1;
        
        return integer ? '1' : '0';
    }
    
    
    /*
     * prima char '0' ili '1'
     * vraca broj iz rijecnika
     */
    unsigned long long int kompresiraj( char noviBit ) {
        
        string skupniSimbol = radnaRijec + noviBit;
        
        // ako skupniSimbol postoji u rjecniku
        if ( rjecnik.count( skupniSimbol ) ) {
            
            radnaRijec = skupniSimbol;
            return -1;
        }
        
        unsigned long long int izlaz = rjecnik[ radnaRijec ];
        
        rjecnik[ skupniSimbol ] = rjecnik.size();
        radnaRijec = noviBit;
        
        return izlaz;
    }
    
    
    void ubaciKodove( vector< int > &kompresirano, unsigned long long int kod ) {
        
        // maska prvih sedam bitova
        unsigned long long int maska = 0x7F;
        
        while ( kod > maska ) {
            
            int noviKod = kod & maska;
            kod >>= 7;
            
            // dodaj osmi bit kao znak da se trenutni kod nastavlja
            kompresirano.push_back( noviKod | 0x80 );
        }
        
        // zadnji dio koda sigurno nece imati osmi bit
        kompresirano.push_back( kod );
        
    }
    
    
    vector< int > lzwKompresija( vector< int > &data ) {
        
        initRjecnik();
        
        // ovo mozda ubacit u init() funkciju
        radnaRijec = "";
        bool pocetak = true;
        
	/*
         * ovdje ce se nalaziti kompresirana poruka
         * vector je zato sto ce niz rasti, pa da ne treba rucno (re)alocirati
	 */
        vector< int > kompresirano;
        
        // pazit na tip data i dobivanje lengtha
        for ( vector< int >::iterator i = data.begin(); i != data.end(); i++ ) {
            
            int trenutniInt = *i;
	    
            // setanje po bitovima
            for ( int j = 13; j >= 0; j-- ) {
                
                char trenutniBit = pronadjiBit( trenutniInt, j );
                
                if ( pocetak ) {
                    radnaRijec = trenutniBit;
                    pocetak = false;
                }
                
                unsigned long long int kod = kompresiraj( trenutniBit );
                
                //pretvori kod u niz brojeva manjih od 256
                //strpaj kod u kompresirano
                if ( kod > -1 ) {
                    ubaciKodove( kompresirano, kod );
                }
            }
        }
        
        // pozovi kompresiju zadnji put da se ispise kod za trenutnu radnu rijec ( ili nova funkcija)
        unsigned long long int kod = kompresiraj( '#' );
        if ( kod > -1 ) {
            ubaciKodove( kompresirano, kod );
        }
        
        if ( radnaRijec.compare( "#" ) ) {
            ubaciKodove( kompresirano, rjecnik["#"] );
        } else {
            cout << "nesto ne valja";
        }
        
        return kompresirano;
    }
};

vector< int > DATCompression::compress( vector< int > data ) {
    
    //maknut X, Y, L, iskoristiti to u svojim varijablama
    //pozovi neku funkciju za pripremit podatke (delte recimo)
    
    vector< int > kompresirano;
    
    kompresirano = lzwKompresija( data );
    
    // ako treba, napravi jos nesto sa kompresiranim podacima (mozda dodaj velicinu na pocetak)
    
    return kompresirano;
}

///////////////////////////////////////////////////////////////////////////////
/*OVO SE NE PREDAJE*/
///////////////////////////////////////////////////////////////////////////////

// splitanje stringova (preuzeto s 1. utr labosa :P )
vector< string > split(const string &mrcina, const string &cepalo) {
    
    vs komadici;
    int mjestoZaCepanje = 0;
    int odmak = 0;
    
    while ( mjestoZaCepanje != string::npos ) {
        
        mjestoZaCepanje = mrcina.find(cepalo, odmak);
        string komad( mrcina, odmak, mjestoZaCepanje - odmak );
        komadici.push_back( komad );
        odmak = mjestoZaCepanje + cepalo.size();
    }
    
    return komadici;
}

//test main - REMOVE BEFORE SUBMITTING
int main( int argc, char *argv[] ) {
    
    ifstream tokTest( argv[1] );
    
    vector< int > ulazniPodaci;
    
    int broj, X = 0, Y = 0, L = 0;
    enum stanja { prviRedak, ostaliRetci };
    stanje ucitavanje = prviRedak;
    string redak;
    vector< string > splitaniRedak;
    
    // postavi dimenzije ulaznih podataka na 0, kasnije ce se promijeniti
    ulazniPodaci.push_back( X );
    ulazniPodaci.push_back( Y );
    ulazniPodaci.push_back( L );
    
    while ( !tokTest.eof() ) {
        getline( tokTest, redak );
        
        splitaniRedak = split( redak, "\t" );
        
        //TODO probni print dal radi split
        
        int trenutniX = atoi( splitaniRedak[0].cstr() );
        int trenutniY = atoi( splitaniRedak[1].cstr() );
        
        if ( trenutniX == X ) {
            X++;
            Y = 0;
        }
        if ( trenutniY == Y ) {
            Y++;
        }
        
        if ( ucitavanje == prviRedak ) {
            L = splitaniRedak.size() - 2;
            ucitavanje = ostaliRetci;
        }
        
        for ( vector< string >::iterator it = splitaniRedak.begin() + 2;
            it != splitaniRedak.end(); it++ ) {
            
            ulazniPodaci.push_back( atoi( (*it).cstr() ) );
        }
    }
    
    ulazniPodaci[0] = X;
    ulazniPodaci[1] = Y;
    ulazniPodaci[2] = L;
    
    DATCompression mojKompresor;
    
    vector< int > tijesno = mojKompresor( ulazniPodaci );
    
    for ( vector< int >::iterator it = tijesno.begin(); it != tijesno.end(); it++ ) {
        
        cout << *it << endl;
    }
    
    return 0;
}