#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>

// <declarations> → VAR <instantiation> FVAR | INSTR <assignation> FINSTR
// <assignation> → <variable> = <or>
// <or> → <and> | <or> | <and>
// <and> → <plusmoins> | <and> & <plusmoins>
// <plusmoins> → <foisdiv> | <plusmoins> + <foisdiv> | <plusmoins> - <foisdiv>
// <foisdiv> → <negnot> | <foisdiv> * <negnot> | <foisdiv> / <negnot>
// <negnot> → <facteur> | -<negnot> | ~<negnot>
// <facteur> → ( <declaration> ) | <nombre>
// <nombre> -> <nombredeci> | 0b<nombrebin> | 0x<nombrehex> | <variable>
// <nombredeci> -> <chiffredeci> | <nombredeci><chiffredeci>
// <Chiffre> -> 0 | 1 | 2 | 3 | 4 | 5 | 6 | 7 | 8 | 9
// <nombrebin> -> <chiffrebin> | <nombrebin><chiffrebin>
// <chiffrebin> -> 0 | 1
// <nombrehex> → <chiffrehex> | <nombrehex><chiffrehex>
//<chiffrehex> → 0 | 1 | 2 | 3 | 4 | 5 | 6 | 7 | 8 | 9 | A | B | C | D | E | F

void Declaration();
void Or();
void And();
void PlusMoins();
void FoisDiv();
void NegNot();
void Facteur();
int Nombre(int);
int NombreDeci();
int NombreBin();
int ChiffreBin();
int NombreHex();
int ChiffreHex();
void CaractereSuivant();
void CaracterePos(int);
char CaractereCourant();

// expression à compiler
char acExpr[100];
char currentVar[3] = {0};
char tmpVar[3];
FILE * fdest;

char bCourant = 0; // indice du caractère courant dans l'expression
int nbPara = 0;

void main()
{
	FILE * fp;

	fp = fopen("source.txt", "r");

	if (fp != NULL)
	{
		
		// Ajouter le contenu du fichier dans acExpr
		fread(acExpr, 1, sizeof(acExpr), fp);
		fclose(fp);

		//printf("%s\n\r", acExpr);

		fdest = fopen("dest.c", "w");

		if (fdest != NULL)
		{

			Declaration(); // démarrer la compilation

			if (CaractereCourant() != 0)
				printf("Le caractere %c a la position %d dans  est invalide!",
					acExpr[bCourant], bCourant + 1);
			else
				printf("La compilation s'est bien deroulee!");

			fclose(fdest);
		}
		else
		{
			printf("Erreur lors de la création du fichier destination.");
		}
	}
	else
	{
		printf("Erreur lors de l'ouverture du fichier source.");
	}
}

// <declarations> → VAR <instantiation> FVAR | INSTR <assignation> FINSTR
// <assignation> → <variable> = <or>
void Declaration()
{
	int val, unsign = 0;

	// Déclaration des variables 
	if (strncmp(&acExpr[bCourant], "VAR", 3) == 0)
	{
		CaracterePos(3);

		do
		{
			if(CaractereCourant() == 'u')
			{
				CaractereSuivant();
				unsign = 1;
			}

			// Déclaration int
			if (CaractereCourant() == 'i')
			{
				CaractereSuivant();

				if (isdigit(CaractereCourant()))
				{
					val = CaractereCourant() - 0x30;

					CaractereSuivant();

					if (CaractereCourant() == '=')
					{
						CaractereSuivant();
						if(unsign == 0)
							fprintf(fdest, "int i%d=%d;\n", val, Nombre(0));
						else
							fprintf(fdest, "unsigned int i%d=%d;\n", val, Nombre(0));
					}
				}
			}

			// Déclaration short
			if (CaractereCourant() == 's')
			{
				CaractereSuivant();

				if (isdigit(CaractereCourant()))
				{
					val = CaractereCourant() - 0x30;

					CaractereSuivant();

					if (CaractereCourant() == '=')
					{
						CaractereSuivant();
						if(unsign == 0)
							fprintf(fdest, "short s%d=%d;\n", val, Nombre(0));
						else
							fprintf(fdest, "unsigned short s%d=%d;\n", val, Nombre(0));
					}
				}
			}

			// Déclaration char
			if (CaractereCourant() == 'b')
			{
				CaractereSuivant();

				if (isdigit(CaractereCourant()))
				{
					val = CaractereCourant() - 0x30;

					CaractereSuivant();

					if (CaractereCourant() == '=')
					{
						CaractereSuivant();
						if(unsign == 0)
							fprintf(fdest, "char b%d=%d;\n", val, Nombre(0));
						else
							fprintf(fdest, "unsigned char b%d=%d;\n", val, Nombre(0));
					}
				}
			}

			if(CaractereCourant() == ',')
				CaractereSuivant();

		} while (strncmp(&acExpr[bCourant], "FVAR", 4) != 0);

		CaracterePos(4);
	}
	
	// Daclaration des instructions
	if (strncmp(&acExpr[bCourant], "INSTR", 5) == 0)
	{
		CaracterePos(5);
		fprintf(fdest, "void main()\n");
		fprintf(fdest, "{\n");
		fprintf(fdest, "  _asm\n");
		fprintf(fdest, "  {\n");

		do
		{
			Or();

			if (currentVar[0] != '\0')
			{
				fprintf(fdest, "    pop eax\n", &currentVar[0]);
				if(currentVar[0] == 'i')
					fprintf(fdest, "    mov %s, eax\n", &currentVar[0]);
				else if(currentVar[0] == 's')
					fprintf(fdest, "    mov %s, ax\n", &currentVar[0]);
				else if(currentVar[0] == 'b')
					fprintf(fdest, "    mov %s, al\n", &currentVar[0]);
			}
			if (CaractereCourant() == ',')
			{
				currentVar[0] = '\0';
				CaractereSuivant();
			}

		} while (strncmp(&acExpr[bCourant], "FINSTR", 4) != 0);

		CaracterePos(6);
		fprintf(fdest, "  }\n");
		fprintf(fdest, "}\n");
	}

}

// <or> → <and> | <or> | <and>
void Or()
{
	And();

	while (CaractereCourant() == '|')
	{
		CaractereSuivant();
		And();

		fprintf(fdest, "    pop ebx\n");
		fprintf(fdest, "    pop eax\n");
		fprintf(fdest, "    or eax, ebx\n");
		fprintf(fdest, "    push eax\n");
	}
}

// <and> → <plusmoins> | <and> & <plusmoins>
void And()
{
	PlusMoins();

	while (CaractereCourant() == '&')
	{
		CaractereSuivant();
		PlusMoins();

		fprintf(fdest, "    pop ebx\n");
		fprintf(fdest, "    pop eax\n");
		fprintf(fdest, "    and eax, ebx\n");
		fprintf(fdest, "    push eax\n");
	}
}

// <plusmoins> → <foisdiv> | <plusmoins> + <foisdiv> | <plusmoins> - <foisdiv>
void PlusMoins()
{
	FoisDiv();

	while (CaractereCourant() == '+' || CaractereCourant() == '-')
	{

		switch (CaractereCourant())
		{
		case '+':
			CaractereSuivant();
			FoisDiv();
			fprintf(fdest, "    pop ebx\n");
			fprintf(fdest, "    pop eax\n");
			fprintf(fdest, "    add eax, ebx\n");
			fprintf(fdest, "    push eax\n");
			break;
		case '-':
			CaractereSuivant();
			FoisDiv();
			fprintf(fdest, "    pop ebx\n");
			fprintf(fdest, "    pop eax\n");
			fprintf(fdest, "    sub eax, ebx\n");
			fprintf(fdest, "    push eax\n");
		}
	}
}

// <foisdiv> → <negnot> | <foisdiv> * <negnot> | <foisdiv> / <negnot>
void FoisDiv()
{
	NegNot();

	while (CaractereCourant() == '*' || CaractereCourant() == '/')
	{
		switch (CaractereCourant())
		{
		case '*':

			CaractereSuivant();
			NegNot();

			fprintf(fdest, "    pop ebx\n");
			fprintf(fdest, "    pop eax\n");
			fprintf(fdest, "    imul eax, ebx\n");
			fprintf(fdest, "    push eax\n");
			break;
		case '/':

			CaractereSuivant();
			NegNot();

			fprintf(fdest, "    pop ebx\n");
			fprintf(fdest, "    pop eax\n");
			fprintf(fdest, "    cdq\n");
			fprintf(fdest, "    idiv ebx\n");
			fprintf(fdest, "    push eax\n");
		}
	}
}

// <negnot> → <facteur> | -<negnot> | ~<negnot>
void NegNot()
{
	if (CaractereCourant() == '-')
	{
		CaractereSuivant();
		Facteur();

		fprintf(fdest, "    pop eax\n");
		fprintf(fdest, "    neg eax\n");
		fprintf(fdest, "    push eax\n");
	}
	else
	{
		if (CaractereCourant() == '~')
		{
			CaractereSuivant();
			Facteur();

			fprintf(fdest, "    pop eax\n");
			fprintf(fdest, "    not eax\n");
			fprintf(fdest, "    push eax\n");
		}
		else
		{
			Facteur();
		}
	}
}

// <facteur> → ( <declaration> ) | <nombre>
void Facteur()
{
	if (CaractereCourant() == '(')
	{
		CaractereSuivant();
		Or();	

		if (CaractereCourant() == ')')
			CaractereSuivant();
	}
	else
		Nombre(1);
}

// <nombre> -> <nombredeci> | 0b<nombrebin> | 0x<nombrehex> | <variable>
int Nombre(int push)
{
	int iVal = 1, unsign = 0;

	// Variable

	if(CaractereCourant() == 'u')
	{
		CaractereSuivant();
		unsign = 1;
	}

	if (strncmp(&acExpr[bCourant], "i", 1) == 0 || strncmp(&acExpr[bCourant], "s", 1) == 0 || strncmp(&acExpr[bCourant], "b", 1) == 0)
	{
		CaractereSuivant();
		if (isdigit(CaractereCourant()))
		{
			tmpVar[0] = acExpr[bCourant-1];
			tmpVar[1] = acExpr[bCourant];

			CaractereSuivant();

			if (CaractereCourant() == '=')
			{
				strcpy(currentVar, tmpVar);
				CaractereSuivant();
				Or();
			}
			else
			{
				
				if(tmpVar[0] != currentVar[0])
				{
					if(unsign)
						fprintf(fdest, "    movzx eax, %s\n", &tmpVar[0]);
					else
						fprintf(fdest, "    movsx eax, %s\n", &tmpVar[0]);
					fprintf(fdest, "    push eax\n");
				}
				else
				{
					fprintf(fdest, "    push %s\n", &tmpVar[0]);
				}

			}	
		}
	}
	
	// Binaire
	else if (strncmp(&acExpr[bCourant], "0b", 2) == 0)
	{
		CaracterePos(2);
		iVal = NombreBin();
		if (push)
			fprintf(fdest, "    push %d\n", iVal);
	}
	// Hexa
	else if (strncmp(&acExpr[bCourant], "0x", 2) == 0)
	{
		CaracterePos(2);
		iVal = NombreHex();
		if (push)
			fprintf(fdest, "    push %d\n", iVal);
	}
	// Décimal
	else
	{
		iVal = NombreDeci();
		if (push)
			fprintf(fdest, "    push %d\n", iVal);
	}

	return iVal;
}

// <nombredeci> -> <chiffredeci> | <nombredeci><chiffredeci>
// <Chiffre> -> 0 | 1 | 2 | 3 | 4 | 5 | 6 | 7 | 8 | 9
int NombreDeci()
{
	int iVal = 0, neg = 0;

	if(CaractereCourant() == '-')
	{
		CaractereSuivant();
		neg = 1;
	}

	if (isdigit(CaractereCourant()) != 0)
	{
		while (isdigit(CaractereCourant()) != 0)
		{
			iVal = (iVal * 10) + (CaractereCourant() - 0x30);
			CaractereSuivant();
		}
	}

	if(neg)
		iVal = -iVal;

	return iVal;
}

// <nombrebin> -> <chiffrebin> | <nombrebin><chiffrebin>
int NombreBin()
{
	int iVal = 0;
	if (ChiffreBin() != 0)
	{
		while (ChiffreBin() != 0)
		{
			iVal = (iVal * 2) + (CaractereCourant() - 0x30);
			CaractereSuivant();
		}
	}
	return iVal;
}

// <chiffrebin> -> 0 | 1
int ChiffreBin()
{
	if (CaractereCourant() == '0' || CaractereCourant() == '1')
		return 1;
	else return 0;
}

// <nombrehex> → <chiffrehex> | <nombrehex><chiffrehex>
int NombreHex()
{
	int iVal = 0;

	while (ChiffreHex() != 0)
	{
		iVal = (iVal * 16) + ChiffreHex();
		CaractereSuivant();
	}
	
	return iVal;
}

//<chiffrehex> → 0 | 1 | 2 | 3 | 4 | 5 | 6 | 7 | 8 | 9 | A | B | C | D | E | F
int ChiffreHex()
{
	
	if (CaractereCourant() >= 48 && CaractereCourant() <= 57)   // entre 0-9
	{
		return (((int)(CaractereCourant())) - 48);
	}
	else if ((CaractereCourant() >= 65 && CaractereCourant() <= 70))   // entre A-F
	{
		return (((int)(CaractereCourant())) - 55);
	}
	else if (CaractereCourant() >= 97 && CaractereCourant() <= 102)   // entre a-f
	{
		return (((int)(CaractereCourant())) - 87);
	}
	else
	{
		return 0; 
	}
}

// Cette fonction passe au caractère suivant dans l'expression
void CaractereSuivant()
{
	do
		bCourant++;
	while (acExpr[bCourant] == ' ' || acExpr[bCourant] == '\n' || acExpr[bCourant] == '\t' || acExpr[bCourant] == '\r');
	// éliminer les espaces
	// retour à la ligne CRLF
	// tabulation
}

// Permet de passe plusieurs caractère
void CaracterePos(int nbel)
{
	int i;
	
	for (i = 0; i < nbel; i++)
	{
		CaractereSuivant();
	}
}

// Cette fonction retourne le caractère courant de l'expression
char CaractereCourant()
{
	return acExpr[bCourant];
}