#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//kelime bilgilerinin tutulacagi struct yapisi
typedef struct {
    char kelime[5]; //kelimenin kendisini tutar
    int *komsu; //komsularini dizi seklinde tutar
    int komsuSayisi; //kac adet komsunu oldugunu tutar
    //alttaki 3 degisken dijkstra için kullanılacak
    int uzaklik ; //baslangictan olan uzaklik
    int secildiMi; //0 ise o graf hic islenmedi, 1 ise komsulari incelendi
    int nerdenGeldi ; //bir önceki grafin indisini tutar, yol için
}KELIME;

int yol[2415]; //hedef kelimeye ulasirken gecilen kelimelerin indislerini tutar
int uzunluk = 0; //yolun uzunlugunu tutar, kac kelimeden gecildi

KELIME * dosyaOku(KELIME *k); //kelimeleri dosyadan okur ve diziye atar
void yaz(KELIME *k); //test amacli! okunan kelimeleri ekrana basar
void komsulukBul(KELIME *k); //her bir kelimeyi tek tek isleyerek komsularini bulur
int komsuMu(KELIME *k, char k1[], char k2[]); //girilen 2 kelimenin komsu olup olmadiklarini tutar
int indisBul(KELIME *k, char kelime[]); //girilen kelimenin indisini bulur
void yolBul(KELIME *k, int indis1, int indis2); //girilen 2 kelime arasindaki donusumu bulur
int yoldaMi(int i); //yolun dongu olusturmamasi icin o kelimenin yolda olup olmadigini kontrol eder
int dijkstra(KELIME *k, int indis1, int indis2); //dijkstra algoritmasi ile en kisa yolu bulur

int main(){
    KELIME *k; //kelime bilgilerini tutan dizinin pointer i
    int i; //dongu indisi
    int indis1; //girilen ilk kelimenin indisi
    int indis2; //girilen ikinci kelimenin indisi
    int secim; //dijkstra mi tum yollar mi ? secimi yapilacakken kullanilan degisken

    k = (KELIME *) malloc (sizeof(KELIME)*2415); //tutulacak kelimeler icin yer ayrildi

    if(k == NULL){ //eger yer ayrilamazsa programi kapat
        printf("Yer ayrilamadi");
        exit(1);
    }

    for(i=0; i<2415; i++){ //komsuluk bilgileri icin ilklendirmelerin yapilmasi
        k[i].komsuSayisi = 0;
        k[i].komsu = (int *) malloc (1*sizeof(int)); //ilk eklenecek komsu icin yer acildi
    }

    k = dosyaOku(k); // kelimeleri oku ve baslangic adresini k pointer ina  at
    //yaz(k); //test amacli oldugundan yoruma alinmistir
    komsulukBul(k); // kelimelerin komsulari bulundu

    komsuMu(k,"level","bevel"); //2 kelimenin komsu olup olmadiklari test edildi
    komsuMu(k,"zippy","tippy"); //2 kelimenin komsu olup olmadiklari test edildi
    indis1 = indisBul(k, "lossy"); //ilk kelimenin indisi bulundu
    indis2 = indisBul(k, "mousy"); //ikinci kelimenin indisi bulundu
    //printf("%d %d",indis1,indis2); //test amacli oldugundan yoruma alinmistir

    printf("Ne yapmak istersiniz?\n");
    printf("1.Tum yollari bul \n2.Dijkstra ile en kisa yolu bul\n");
    scanf("%d",&secim);
    if(secim == 1)
        yolBul(k, indis1, indis2);
    else
        dijkstra(k, indis1, indis2);

    return 0;
}

int dijkstra(KELIME *k, int indis1, int indis2){
    int indis; // ele alinanacak indis
    int i; //dongu indisi
    int j = 0; //dongu indisi
    int s = 0; //gecilen kelime sayisi

    for(i=0; i<2415; i++){ //dijkstra icin ilklendirmeleri yap
        k[i].uzaklik = 10000; //max uzaklik olarak belirlendi
        k[i].secildiMi = 0; //baslangicta hic bir dugum secilmemistir
        k[i].nerdenGeldi = -2; //onceki indisleri belirsizdir
    }

    //baslangic indisi icin ilklendirmeler
    k[indis1].uzaklik = 0; //kendine uzakligi 0
    k[indis1].nerdenGeldi = -1; //baslangic indisinden oncesi yok

    for(j=0; j<2415; j++){ //tum dugumler islenene kadar don
        indis = minBul(k); //uzakligi en kucuk dugumu dondur
        k[indis].secildiMi = 1; //en yakın graf secilmis oldu

        //o indisin komsularinin uzakliklarini guncelle
        for(i=0; i<k[indis].komsuSayisi; i++){ //o anki dugumun tum komsulari kadar don
            //eger komsu dugumlere daha kisa ulasilabiliniyorsa uzakliklari guncelle
            if(k[indis].uzaklik+1<k[k[indis].komsu[i]].uzaklik){
                k[k[indis].komsu[i]].uzaklik = k[indis].uzaklik;
                k[k[indis].komsu[i]].nerdenGeldi = indis;
            }
        }

        //hedef dugumun onceki indisi belirsizse hedefe yol yok demektir
        if(k[indis2].nerdenGeldi == -2 && j==2413){
            printf("Yol Bulunamadi");
            return -1;
        }
    }


    //dijkstra ile bulunan yolu yazdir
    i = indis2;
    do{
        s++;
        printf("%s\t",k[i].kelime);
        i = k[i].nerdenGeldi;
    }while(k[i].nerdenGeldi!=-1); //baslangic dugumunun onceki indisi -1 olarak atanmisti
    s++;
    printf("%s\t",k[i].kelime); //while da erisilemeyen son dugumu yazdir
    printf("\nYolun uzunlugu:%d",s);
}

int minBul(KELIME *k){ //Dijkstra da islenmesi icin uzakligi en kucuk dugumu bul ve dondur
    int minIndis; //bulunacak minIndis
    int i; //dongu indisi

    minIndis = 0; //ilk indisi min sec

    for(i=1; i<2415; i++){ //tum dugumleri dolas
        //uzakligi ne kucuk olan ve secilmemis dugumu al
        if(k[i].uzaklik<k[minIndis].uzaklik && k[i].secildiMi == 0)
            minIndis = i;
    }

    return minIndis;
} //minBul() sonu

KELIME * dosyaOku(KELIME *k){
    FILE *fp; //FILE pointer i
    int i = 0; //dongu indisi

    fp = fopen("kelime.txt","r");

    if(fp == NULL){ //dosya acma hatasi olursa programi sonlandir
        printf("Dosya acilamadi");
        exit(1);
    }

    do{
        fscanf(fp,"%s",k[i].kelime);
        i++;
    }while(!feof(fp)); //dosya sonuna kadar kelimeleri oku ve ata

    return k;
} //dosyaOku() sonu

int indisBul(KELIME *k, char kelime[]){ //istenen kelimenin indisini bulur
    int i = 0;

    while(i<2415 && strcmp(kelime,k[i].kelime)!=0)
        i++;

    if(i<2415){
        printf("kelime mevcut\n");
        return i;
    }
    else{
        printf("mevcut degil\n");
        return -1;
    }
} //indisBul() sonu

//indis2 sabit, hedef dugum, indis1 fonksiyonda surekli degisen, o anki dugum
void yolBul(KELIME *k, int indis1, int indis2){ //mevcut tum yollari bulur

    if(yoldaMi(indis1)==0){ //kelime yolda mecvut degilse yola eklenir
        yol[uzunluk] = indis1;
        uzunluk++;
    }

    int i = 0; //dongu indisi
    int p; //dongu indisi

    if(indis1 == indis2){ // hedef dugume ulasildiMi?
        printf("\nYolun uzunlugu:%d",uzunluk);
        for(p=0; p<uzunluk; p++){ //hedef dugume ulasildigi an yolu yazdir
            printf("\t%s",k[yol[p]].kelime);
         }

        uzunluk--; //yolu bir azaltarak yeni denemelerin ustune yazilmasini sagla

        // exit(1);
         puts("\n");
    }
    else{ //hedefe henuz ulasilamadi
        for(i = 0; i<k[indis1].komsuSayisi; i++){ //o anki dugumun tum komsularina bak
            if(yoldaMi(k[indis1].komsu[i])==0){ //yolda olmayan komsularini sec ve yola devam et
                yolBul(k,k[indis1].komsu[i],indis2);

            }
        }
        //tum komsulari dolandiysa yolu bir azalt
        if(i == k[indis1].komsuSayisi)
            uzunluk--;
    }

} //yolBul() sonu

int yoldaMi(int i){ //istenen kelimenin yola eklenip eklenmedigine bakar
    int j = 0; //dongu indisi

    while(j<uzunluk && i != yol[j])
        j++;

    if(j==uzunluk)//yolda degil
        return 0;
    else
        return 1;

} //yoldaMi() sonu

void komsulukBul(KELIME *k){ //tum kelimelerin tum komsularini bulur
    //dongu indisleri
    int i; //komsulugu bulunacak kelimenin indisi
    int j; //komsu olup olmadigi denenecek kelimelerin indisleri
    int p; //kelimelerin harfleri karsilastirilirken kullanilan indis
    int s; //farkli harfleri sayan degisken

    for(i=0; i<2414; i++){ //her kelime icin
        for(j=i+1; j<2415; j++){ // diger kelimelerle komsu olup olmadiklarini kontrol et
            p = 0;
            s = 0;
            while(p<5){ //harf sayisinca don
                if(k[i].kelime[p] != k[j].kelime[p] ){ //farkli harfleri say
                    s++;
                }
                 p++;
            }

            if(s == 1 ){//1 harf farkli, baglanti var
                k[i].komsu[k[i].komsuSayisi] = j; //birbirlerini komsu olarak ekle
                k[j].komsu[k[j].komsuSayisi] = i; //geriye donme ihtiyaci olmamasi icin diger dugum icin de ayni seyi yap

                k[i].komsuSayisi++; // iki dugumun de komsu sayilarini artir
                k[j].komsuSayisi++;

                //siradaki komsular icin yer ac
                k[i].komsu = (int *) realloc(k[i].komsu,(k[i].komsuSayisi+1)*sizeof(int));
                k[j].komsu = (int *) realloc(k[j].komsu,(k[j].komsuSayisi+1)*sizeof(int));
            }
        } //ic for sonu
    } // dis for sonu
} //komsulukBul() sonu

//girilen iki kelimenin komsu olup olmadiklarini bulur
int komsuMu(KELIME *k, char k1[], char k2[]){
    int i,j; //dongu indisleri

    //ilk kelimenin listede mevcut olup olmadiginin kontrolu
    i = 0;
    while(i<2415 && strcmp(k[i].kelime,k1)!=0)
        i++;

    if(i == 2415){
        printf("ilk kelime mevcut degil\n");
        return -1;
    }

    //ikinci kelimenin listede mevcut olup olmadiginin kontrolu
    j = 0;
    while(j<2415 && strcmp(k[j].kelime,k2)!=0)
        j++;


    if(j == 2415){
        printf("ikinci kelime mevcut degil\n");
        return -1;
    }

    //kelimelerin var olduklari anlasildiktan sonra komsuluk kontrolu
    int p = 0; //dongu indisi
    while(p<k[i].komsuSayisi && j!=k[i].komsu[p])
        p++;

    if(p<k[i].komsuSayisi){
        printf("kelimeler komsu\n");
        return 1;
    }
    else{
        printf("kelimeler komsu degil\n");
        return 0;
    }
}

//test amacli eklenen bu fonksiyon okunan tum kelimeleri ekler
void yaz(KELIME *k){
    int i;

    for(i=0; i<2415; i++)
        printf("\n%s",k[i].kelime);

}
