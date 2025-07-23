#include<iostream>

class filtrkalmana {
    private:
        float estym;
        float estym_pop;
        float BLAD;
        float ostatni_pomiar;
        
        float SPR;
        float SPO;
        
        int ilosc;

        public:
        filtrkalmana() : ilosc(0),estym(0),SPR(0),SPO(0),BLAD(1) {}
        float dodaj(float wart){
            if(ilosc==0){
                estym = wart;
            }
            else{
                float x=ostatni_pomiar-wart;
               
                SPO=(SPO*ilosc+x*x)/(ilosc+1);

                float estym_przew=estym;
                float BLAD_przew=BLAD+SPR;
                float wzmocnienie=9999999;
                if(BLAD_przew+SPR!=0)
                    wzmocnienie=BLAD_przew/(BLAD_przew+SPR);

                estym=estym_przew+wzmocnienie*(wart-estym_przew);
                BLAD=(1-wzmocnienie)*BLAD_przew;

                float z=estym-estym_pop;
                SPR=(SPR*ilosc+z*z)/(ilosc+1);
                
            }
            estym_pop=estym;
            ostatni_pomiar = wart;
            ilosc++;
            return estym;
        }
};

int main(){
    float f,wyniki[3000];
    int x=0;
    filtrkalmana *kalman=new filtrkalmana();
    
    while(std::cin>>f)
    {
        std::cout<<kalman->dodaj(f)<<"\n";
    }
    
}