#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <Windows.h>
#include <unistd.h>
#define NB_DOM 28
#define NB_INITIAL_DOM 7 // A baisser si le NB_JOUEURS augmente
#define NB_JOUEURS 1    // 4 joueurs max
#define PIOCHE 0
#define ORDI 9
#define PLAYER1 1

typedef struct
{
    int symbole[NB_DOM][2]; //Dessins sur chaque domino
    int plateau[NB_DOM];  //0=pioche, 1=joueur1,9=ordi
}T_domino;


typedef struct
{
    int dispo[NB_DOM]; // 1 = le joueur X possede de ce domino, 0 si il ne le possede pas
    int nb_dominos;
}T_jeu;

void afficher_regles(void);
void srand(unsigned int Seed);
//void sleep(unsigned int); 


void init_var(T_jeu*pioche);
void generation_aleatoire(T_jeu*pioche,T_jeu*player1,T_jeu*ordi,T_domino*game);
int who_starts1(T_jeu*player1,T_jeu*ordi,T_jeu*pioche,T_domino*game,int pioche_player1,int passe_player1);
int who_starts2(T_jeu*player1,T_jeu*ordi,T_jeu*pioche,T_domino*game,int somme_player1, int somme_ordi,int pioche_player1,int passe_player1);
int calcul_somme(T_domino*game,int somme_player1,int somme_ordi);
void afficher_dom(T_jeu*player1,T_domino*game);
void compare_nb_dom(T_jeu*player1,T_jeu*ordi,T_domino*game,int fin,int somme_player1,int somme_ordi);
void end_game(T_jeu*player1,T_jeu*ordi);


int start_ordi(T_jeu*ordi,T_domino*game);
int verif_comp_ordi(T_jeu*ordi,T_domino*game,T_jeu*pioche,int pioche_ordi,int passe_ordi);
void piocher_ordi(T_domino*game,T_jeu*pioche,T_jeu*ordi,int pioche_ordi,int passe_ordi);
void passer_ordi(int passe_ordi);


int start_player1(T_jeu*player1,T_jeu*ordi,T_jeu*pioche,T_domino*game,int pioche_player1,int passe_player1);
void tour_player1(T_jeu*player1,T_domino*game,T_jeu*pioche,T_jeu*ordi,int pioche_player1,int passe_player1);
void jouer_player1(T_jeu*player1,T_domino*game,T_jeu*pioche,T_jeu*ordi,int pioche_player1,int passe_player1);
int piocher_player1(T_jeu*player1,T_domino*game,T_jeu*pioche,T_jeu*ordi,int pioche_player1,int passe_player1);
void passer_player1(int passe_player1);





int main(void)
{
    T_domino game=
     {
        {0,0,0,1,0,2,0,3,0,4,0,5,0,6,1,1,1,2,1,3,1,4,1,5,1,6,2,2,2,3,2,4,2,5,2,6,3,3,3,4,3,5,3,6,4,4,4,5,4,6,5,5,5,6,6,6},
        {0}
     };
    T_jeu ordi={0};
    T_jeu player1={0};
    T_jeu pioche={0};

    int fin=0;

    int pioche_player1=0;
    int pioche_ordi=0;

    int passe_player1=0;
    int passe_ordi=0;

    int somme_ordi=0;
    int somme_player1=0;

    afficher_regles();

    srand( (unsigned int)time(NULL));
    init_var(&pioche);
    generation_aleatoire(&pioche,&player1,&ordi,&game);

    if (who_starts1(&player1,&ordi,&pioche,&game,pioche_player1,passe_player1) == 0) //Comparaison des couples, si un couple est détecté:return 1,sinon 0
    {
        who_starts2(&player1,&ordi,&pioche,&game,somme_player1,somme_ordi,pioche_player1,passe_player1); //aucun couple détecté, on calcule donc la somme
    }

    if(player1.nb_dominos == NB_INITIAL_DOM -1)
    {
        while((player1.nb_dominos != 0)||(ordi.nb_dominos != 0)||((passe_ordi == 1)&&(passe_player1) == 1))
        {
            verif_comp_ordi(&ordi,&game,&pioche,pioche_ordi,passe_ordi);
            tour_player1(&player1,&game,&pioche,&ordi,pioche_player1,passe_player1);
        }
   }
    else if(ordi.nb_dominos == NB_INITIAL_DOM -1)
    {
        while((player1.nb_dominos != 0)||(ordi.nb_dominos != 0)||((passe_ordi == 1)&&(passe_player1) == 1))
        {
           tour_player1(&player1,&game,&pioche,&ordi,pioche_player1,passe_player1);
           verif_comp_ordi(&ordi,&game,&pioche,pioche_ordi,passe_ordi);
        }
    }
    if((passe_ordi == 1)&&(passe_player1 == 1))
    {
        compare_nb_dom(&player1,&ordi,&game,fin,somme_player1,somme_ordi);
    }
    else
    {
        end_game(&player1,&ordi);
    }
    return 0;
}


void init_var(T_jeu*pioche)
{
  pioche->nb_dominos=NB_DOM;

  for(int k=0;k<NB_DOM;k++)
  {
    pioche->dispo[k]=1;
  }
}


void generation_aleatoire(T_jeu*pioche,T_jeu*player1,T_jeu*ordi,T_domino*game)
{
    /* On distribue aleatoirement Nombre_initial_dom a chaque joueur. Ici (7)
    Lorsqu'un domino est ditribue,il devient disponible dans la vue du joueur qui le recoit.
    Le plateau recoit l'information que le domino appartient a ce joueur
    Le domino distribue est retire de la pioche.*/
    int n=0;
    while(n<NB_INITIAL_DOM)
    {
        int a = rand()%NB_DOM;
        if(game->plateau[a]==PIOCHE)
        {
            player1->dispo[a]=1;
            player1->nb_dominos++;

            game->plateau[a]=PLAYER1;

            pioche->dispo[a]=0;
            pioche->nb_dominos--;
            n++;
        }
    }
    n=0;
    while(n<NB_INITIAL_DOM)
    {
        int b=rand()%NB_DOM;
        if(game->plateau[b]==PIOCHE)
        {
            ordi->dispo[b]=1;
            ordi->nb_dominos++;
            game->plateau[b]=ORDI;
            pioche->dispo[b]=0;
            pioche->nb_dominos--;
            n++;
        }
    }
    return ;
}


int who_starts1(T_jeu*player1,T_jeu*ordi,T_jeu*pioche,T_domino*game,int pioche_player1,int passe_player1)
{
    /* On detecte les couples de domino puis on les compare entre les joueurs.
    Celui qui a le plus grand couple commence et on sort de la fonction,si aucun couple n'est detecte,
    on rentre dans la seconde fonction who_starts2 pour compter le nombre de points.*/
    int j=0;
    int couple_ordi[7]={-1,-1,-1,-1,-1,-1,-1};
    int couple_player1[7]={-1,-1,-1,-1,-1,-1,-1};
    for (int i=0;i<NB_DOM;i++)
    {
        //detection des couples du PLAYER1 et stockage dans un tableau
        if(game->plateau[i]==PLAYER1)
        {
            if((game->symbole[i][0])==(game->symbole[i][1]))
            {
                couple_player1[j] = game->symbole[i][0];
                //player1->couple[j][0] = game->symbole[i][0];
                //player1->couple[j][1] = game->symbole[i][1];
                j++;
            }
        }
        //detection des couples de l'ordi et stockage dans un tableau
        else if(game->plateau[i]==ORDI)
        {
            if((game->symbole[i][0])==(game->symbole[i][1]))
            {
                couple_ordi[j] = game->symbole[i][0];
                //ordi->couple[j][0] = game->symbole[i][0];
                //ordi->couple[j][1] = game->symbole[i][1];
                j++;
            }
        }
    }
    j=0;
    /*On deplace le plus grand couple vers la premiere case du tableau,plus pratique pour comparer apres
    On compare la case j avec la case 0, si l'element dans j est superieur a l'element dans 0,
    on affecte la valeur qu'il y a dans j a 0 */
    for(j=0;j<7;j++)
    {
        if(couple_player1[0] < couple_player1[j])
        {
            couple_player1[0] = couple_player1[j];
        }
        else if(couple_ordi[0] <  couple_ordi[j])
        {
            couple_ordi[0]=couple_ordi[j];
        }
    }
    //Maintenant on peut comparer la case 0 des joueurs.
    if((couple_player1[0] > couple_ordi[0]) && (couple_player1[0] > -1))
    {
        printf("Wow, vous possedez le couple le plus eleve: (%d , %d) , c'est donc vous qui commencez! \n",couple_player1[0],couple_player1[0]);
        start_player1(player1,ordi,pioche,game,pioche_player1,passe_player1);
        return 1;
    }
    else if((couple_ordi[0] > couple_player1[0]) && (couple_ordi[0] > -1))
    {
        printf("L'ordi possede le couple le plus eleve: (%d , %d) , c'est donc lui qui commence! \n",couple_ordi[0],couple_ordi[0]);
        start_ordi(ordi,game);
        return 1;
    }
    else
    {
        printf("Personne ne possede de couple,le joueur ayant le plus grand nombre de points commencera \n");
        return 0;
    }
}

int who_starts2(T_jeu*player1,T_jeu*ordi,T_jeu*pioche,T_domino*game,int somme_player1, int somme_ordi,int pioche_player1,int passe_player1)
{
    //Compare la somme la plus grande pour savoir qui commence.
    if (calcul_somme(game,somme_player1,somme_ordi) == ORDI)
    {
        printf("C'est donc l'ordi qui commence \n");
        start_ordi(ordi,game);
        return 1;
    }
    printf("C'est donc vous qui commencez \n");
    start_player1(player1,ordi,pioche,game,pioche_player1,passe_player1);
    return 0;
}



int calcul_somme(T_domino*game,int somme_player1,int somme_ordi)
{
    //Pour chaque domino possede, on ajoute sa valeur a la somme du joueur x
    for(int i =0;i<NB_DOM;i++)
    {
        if(game->plateau[i]==PLAYER1)
        {
            somme_player1 = somme_player1+game->symbole[i][0]+game->symbole[i][1];
        }
        else if(game->plateau[i]==ORDI)
        {
            somme_ordi = somme_ordi + game->symbole[i][0]+game->symbole[i][1];
        }
    }
    if (somme_ordi > somme_player1)
    {
        printf("La somme de vos points est de %d \n",somme_player1);
        printf("La somme de l'ordi est de %d \n",somme_ordi);
        return ORDI;
    }
    else if(somme_player1 > somme_ordi)
    {
        printf("La somme de vos points est de %d \n",somme_player1);
        printf("La somme de l'ordi est de %d \n",somme_ordi);
        return PLAYER1;
    }
    else if(somme_player1 == somme_ordi)
    {
        printf("La somme de vos points est de %d \n",somme_player1);
        printf("La somme de l'ordi est de %d \n",somme_ordi);
        printf("Il y a egalite, incroyable, cest rare ! Je vous laisse la chance de commencer :) \n");
        return PLAYER1;
    }

    return 0;

}

int start_ordi(T_jeu*ordi,T_domino*game)
{
    int i;
    for(i=0;i<28;i++)
    {
        if (ordi->dispo[i]==1)             //l'ordi joue le premier domino qu'il possede
        {
            ordi->dispo[i]=0;
            ordi->nb_dominos--;
            printf("L'ordi a joue le domino (%d , %d) \n", game->symbole[i][0],game->symbole[i][1]);
            game->plateau[i]=-1;
            return 1;
        }
    }
    return 0;
}

int start_player1(T_jeu*player1,T_jeu*ordi,T_jeu*pioche,T_domino*game,int pioche_player1,int passe_player1)
{
    int n;
    printf("C'est a vous de commencer,posez un domino \n");
    afficher_dom(player1,game);
    printf("Pour y acceder,tapez leur numero correspondant \n");

    scanf("\n %id \n ", &n);
    if ((player1->dispo[n]==1)&&(game->plateau[n]==PLAYER1))
    {
        player1->dispo[n]=0;
        player1->nb_dominos--;
        printf("\n Vous avez joue le domino (%d , %d) \n",game->symbole[n][0],game->symbole[n][1]);
        game->plateau[n]=-1;
        return 1;
    }
    else if((player1->dispo[n]==0)||(game->plateau[n]!=PLAYER1))
    {
        printf("Vous ne possedez pas ce domino... Veuillez recommencer");
        tour_player1(player1,game,pioche,ordi,pioche_player1,passe_player1);
        return 0;
    }
return 1;
}

void afficher_dom(T_jeu*player1,T_domino*game)
{
    int i = 0;
    printf("\n Voici les dominos que vous possedez \n");
    for (i=0;i<NB_DOM;i++)
    {
       if ((game->plateau[i] == PLAYER1)&&(player1->dispo[i]==1))
        {
            printf("[%d](%d , %d)  ",i,game->symbole[i][0],game->symbole[i][1]);
        }
    }
    printf("\n");
    return ;
}


void tour_player1(T_jeu*player1,T_domino*game,T_jeu*pioche,T_jeu*ordi,int pioche_player1,int passe_player1)
{
    int choix,pick;
    afficher_dom(player1,game);
    printf("Tapez votre choix (jouer,piocher ou passer) \n");
    scanf("%d",&choix);

    if (choix == 1)
    {
        jouer_player1(player1,game,pioche,ordi,pioche_player1,passe_player1);
    }
    else if(choix == 2)
    {
        if(pioche_player1 == 0)
        {
            piocher_player1(player1,game,pioche,ordi,pioche_player1,passe_player1);
        }
        else if(pioche_player1 == 1)
        {
            printf("Vous avez deja pioche, jouez ou passez votre tour ! \n");
            printf("Tapez votre choix, 0 pour passer ou 1 pour jouer! \n");
            while((pick != 0)&&(pick != 1))
            {
                scanf("%d",&pick);
                if (pick == 0)
                {
                 passer_player1(passe_player1);
                }
                else if (pick == 1)
                {
                 jouer_player1(player1,game,pioche,ordi,pioche_player1,passe_player1);
                }
                else
                {
                printf("Ce n'est pas possible.Tapez 0 ou 1 ! \n");
                }
            }
        }
    }
    else if(choix == 0)
    {
        passer_player1(passe_player1);
    }
    else
    {
       printf("\n Votre choix n'est pas une option,veuillez recommencer \n");
       tour_player1(player1,game,pioche,ordi,pioche_player1,passe_player1);
    }
    passer_player1(passe_player1);
}

void jouer_player1(T_jeu*player1,T_domino*game,T_jeu*pioche,T_jeu*ordi,int pioche_player1,int passe_player1)
{
    int n;
    printf("Choisissez le domino a jouer\n");
    scanf("%id",&n);
    if ((player1->dispo[n]==1)&&(game->plateau[n]==PLAYER1))
    {
        for(int k =0;k<28;k++)
        {
            if ((game->symbole[n][0] == game->symbole[k][0])&&(game->plateau[k]== -1))
            {
                printf("\n Vous avez joue le domino (%d , %d) \n",game->symbole[n][0],game->symbole[n][1]);
                game->symbole[n][0]=-1;
                game->symbole[k][0]=-1;
                game->plateau[n]=-1;

                player1->dispo[n]=0;
                player1->nb_dominos--;
                return ;
            }
            else if ((game->symbole[n][0] == game->symbole[k][1])&&(game->plateau[k]== -1))
            {
                printf("\n Vous avez joue le domino (%d , %d) \n",game->symbole[n][0],game->symbole[n][1]);
                game->symbole[n][0]=-1;
                game->symbole[k][1]=-1;
                game->plateau[n]=-1;

                player1->dispo[n]=0;
                player1->nb_dominos--;
                return ;
            }
            else if ((game->symbole[n][1] == game->symbole[k][0])&&(game->plateau[k]== -1))
            {
                printf("\n Vous avez joue le domino (%d , %d) \n",game->symbole[n][0],game->symbole[n][1]);
                game->symbole[n][1]=-1;
                game->symbole[k][0]=-1;
                game->plateau[n]=-1;

                player1->dispo[n]=0;
                player1->nb_dominos--;
                return ;
            }
            else if ((game->symbole[n][1] == game->symbole[k][1])&&(game->plateau[k]== -1))
            {
                printf("\n Vous avez joue le domino (%d , %d) \n",game->symbole[n][0],game->symbole[n][1]);
                game->symbole[n][1]=-1;
                game->symbole[k][1]=-1;
                game->plateau[n]=-1;

                player1->dispo[n]=0;
                player1->nb_dominos--;
                return ;
            }
        }
        printf("Impossible de placer ce domino,posez en un autre, piochez ou passez!\n");
        tour_player1(player1,game,pioche,ordi,pioche_player1,passe_player1);
        return;
     }
    else if((player1->dispo[n]==0)||(game->plateau[n]!=PLAYER1))
    {
        printf("Vous ne possedez pas ce domino... Veuillez recommencer \n");
        tour_player1(player1,game,pioche,ordi,pioche_player1,passe_player1);
        return;
    }
    passer_player1(passe_player1);
}

int verif_comp_ordi(T_jeu*ordi,T_domino*game,T_jeu*pioche,int pioche_ordi,int passe_ordi)
{
    for (int i =0;i<28;i++)
    {
        if((ordi->dispo[i]==1)&&(game->plateau[i]==ORDI)) //double verification
        {
            for(int k =0;k<28;k++)
            {
                if ((game->symbole[i][0] == game->symbole[k][0])&&(game->plateau[k]== -1))
                {
                    printf("\n L'ordi a joue le domino [%d](%d , %d) \n",i,game->symbole[i][0],game->symbole[i][1]);
                    game->symbole[i][0]=-1;
                    game->symbole[k][0]=-1;
                    game->plateau[i]=-1;

                    ordi->dispo[i]=0;
                    ordi->nb_dominos--;
                    return 0;
                }
                else if ((game->symbole[i][0] == game->symbole[k][1])&&(game->plateau[k]== -1))
                {
                    printf("\n L'ordi a joue le domino [%d](%d , %d) \n",i,game->symbole[i][0],game->symbole[i][1]);
                    game->symbole[i][0]=-1;
                    game->symbole[k][1]=-1;
                    game->plateau[i]=-1;

                    ordi->dispo[i]=0;
                    ordi->nb_dominos--;
                    return 0;
                }
                else if ((game->symbole[i][1] == game->symbole[k][0])&&(game->plateau[k]== -1))
                {
                    printf("\n L'ordi a joue le domino [%d](%d , %d) \n",i,game->symbole[i][0],game->symbole[i][1]);
                    game->symbole[i][1]=-1;
                    game->symbole[k][0]=-1;
                    game->plateau[i]=-1;

                    ordi->dispo[i]=0;
                    ordi->nb_dominos--;
                    return 0;
                }
                else if ((game->symbole[i][1] == game->symbole[k][1])&&(game->plateau[k]== -1))
                {
                   printf("\n L'ordi a joue le domino [%d](%d , %d) \n",i,game->symbole[i][0],game->symbole[i][1]);
                   game->symbole[i][1]=-1;
                   game->symbole[k][1]=-1;
                   game->plateau[i]=-1;

                   ordi->dispo[i]=0;
                   ordi->nb_dominos--;
                   return 0;
                }
            }
            }
        }
    if(pioche_ordi==0)
    {
        printf("l'ordi pioche...");
        piocher_ordi(game,pioche,ordi,pioche_ordi,passe_ordi);
        return 1;
    }
    else if(pioche_ordi==1)
    {
        printf("l'ordi doit passer son tour \n");
        passer_ordi(passe_ordi);
    }
}


void compare_nb_dom(T_jeu*player1,T_jeu*ordi,T_domino*game,int fin,int somme_player1,int somme_ordi)
{
if (ordi->nb_dominos>player1->nb_dominos)
{
    printf("Vous avez gagne la partie , bien joue!");
}
else if (ordi->nb_dominos<player1->nb_dominos)
{
    printf("l'ordinateur a gagne... dommage ");
}
else if (ordi->nb_dominos==player1->nb_dominos)
{
    if (calcul_somme(game,somme_player1,somme_ordi)==somme_ordi)
    {
           printf("L'ordinateur a gagne... dommage");
    }
    else if (calcul_somme(game,somme_player1,somme_ordi)==somme_player1)
    {
        printf("Vous avez gagne la partie, bien jouer!");
    }
    else if (calcul_somme(game,somme_player1,somme_ordi)== 0)
    {
        printf("Vous n'etes vraiment pas chanceux.");
    }
}
}


int piocher_player1(T_jeu*player1,T_domino*game,T_jeu*pioche,T_jeu*ordi,int pioche_player1,int passe_player1)
{
    int i =0;
    {
        for(i=0;i<28;i++)
        {
            if(pioche->dispo[i]==1)
            {
                player1->dispo[i]=1;
                player1->nb_dominos++;

                pioche->dispo[i]=0;
                pioche->nb_dominos--;
                game->plateau[i]=PLAYER1;
                printf("Vous avez pioche le domino [%d](%d , %d)\n",i,game->symbole[i][0],game->symbole[i][1]);
                pioche_player1=1;
                tour_player1(player1,game,pioche,ordi,pioche_player1,passe_player1);
                return 0;
             }
         }
    printf("oups,la pioche est vide,passez votre tour \n");
    passer_player1(passe_player1);
    return 0;
    }
}

void piocher_ordi(T_domino*game,T_jeu*pioche,T_jeu*ordi,int pioche_ordi,int passe_ordi)
{
    int i =0;
    for(i=0;i<28;i++)
    {
        if(pioche->dispo[i]==1)
        {
            ordi->dispo[i]=1;
            ordi->nb_dominos++;

            pioche->dispo[i]=0;
            pioche->nb_dominos--;
            game->plateau[i]=ORDI;
            printf("l'ordi a pioche le domino [%d](%d , %d)",i,game->symbole[i][0],game->symbole[i][1]);
            pioche_ordi=1;
            verif_comp_ordi(ordi,game,pioche,pioche_ordi,passe_ordi);
            return ;
        }
    }
    printf("oups,la pioche est vide,l'ordi passe son tour");
    passer_ordi(passe_ordi);
    return;
}

void passer_player1(int passe_player1)
{
    passe_player1=1;
}
void passer_ordi(int passe_ordi)
{
    passe_ordi=1;
}

void end_game(T_jeu*player1,T_jeu*ordi)
{
    if (player1->nb_dominos == 0)
    {
        printf("\n\n\n\n                BRAVO VOUS AVEZ GAGNE! ");
    }
    else if(ordi->nb_dominos ==0)
    {
        printf("\n\n\n\n                QUEL DOMMAGE,PERDRE CONTRE UN ORDI AUSSI MAL CODE HAHAHA");
    }
}

void afficher_regles(void)
{
    int start = 0;
    while(start != 1)
    {
        printf("Bienvenue dans mon programme chere aventurier/aventuriere! \n\n\n");
        printf("Ce jeu est un jeu de hasard, mais aussi de strategie. \n");
        printf("Le but du jeu est de se debarrasser de tous ses dominos avant l'adversaire. \n");
        printf("Au depart, vous allez recevoir 7 dominos tires totalement au hasard.  \n");
        printf("Chaque domino est caracterise par son numero (de 0 a 27) et par ses deux symboles (de 0 a 6). \n");
        printf("\n\n\n\nATTENTION! Pour selectionner un domino,il faut taper son numero au clavier et valider en appuyant sur ENTER. \n\n");
        printf("Lors de votre tour, tapez 1 pour jouer un domino,\n                    tapez 2 pour piocher un domino\n                    et tapez 0 pour passer votre tour\n");
        printf("Le joueur ayant le double le plus eleve commence.\nSi personne n'a de double,c'est celui qui a la somme de points la plus elevee qui commence \n");
        printf("Le joueur suivant doit poser un domino ayant au moins un des deux symboles identique au precedent.\n");
        printf("Le premier joueur n'ayant plus de domino gagne!\n");
        printf("Si la partie est bloquee, c'est le joueur qui a le moins de dominos qui gagne.\nEn cas d'egalite, le joueur ayant la somme de points la moins elevee gagne\n");

        printf("\n\n\nSi vous avez bien compris,commencez la partie en appuyant sur la touche 1 \n");
        scanf("%d",&start);
        system("cls");
        if(start != 1)
        {
            Sleep(2000);
            printf("Vous avez perdu avant meme d'avoir joue, bravo! \n");
            printf("\n");
            Sleep(1000);
            printf("\n Redemarrage du jeu ... \n");
            Sleep(1000);
            system("cls");
        }
    }
    printf("Parfait!");
    Sleep(2000);
    system("cls");
    printf("                                                       BONNE CHANCE \n");
    printf("Distribution des dominos \n");
    printf("\n");
    Sleep(3000);
}












