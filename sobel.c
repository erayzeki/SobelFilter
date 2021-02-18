#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<math.h>


// function to calculate Gx and Gy value of a pixel and return the new calculated value
int newValue(int row, int col, int mx[3][3], int my[3][3], int **array, int **Gx, int **Gy){
	int matrix[3][3];
	int i, j;
	
	for (i = 0; i < 3; i++){
		for (j = 0; j < 3; j++){
			matrix[i][j] = array[row-1+i][col-1+j];
		}
	}
	
	int gx = 0;
	int gy = 0;
	for (i = 0; i < 3; i++){
		for (j = 0; j < 3; j++){
			gx += matrix[i][j]*mx[i][j];
			gy += matrix[i][j]*my[i][j];
		}
	}
	
	Gx[row][col] = gx;
	Gy[row][col] = gy;
	
	return sqrt(gx*gx + gy*gy);	
}

// function to skip comments on a pgm file
void SkipComments(FILE *fp){
    int ch;
    char line[100];
    while ((ch = fgetc(fp)) != EOF && isspace(ch)) {
        ;
    }
 
    if (ch == '#') {
        fgets(line, sizeof(line), fp);
        SkipComments(fp);
    } else {
        fseek(fp, -1, SEEK_CUR);
    }
} 

// function to read a pgm file and store the pixel values on a 2D array
void readPGM(char *file, int **array){
	int i, j;
	
	FILE *image;
	int ROW, COL, max;
	char version[3];
	image = fopen(file, "rb");
	
	if (image == NULL){
		printf("File cannot be opened!");
		exit(0);
	}
	
	fgets(version, sizeof(version), image);
	SkipComments(image);
	fscanf(image, "%d %d", &COL, &ROW);
	SkipComments(image);
	fscanf(image, "%d", &max);
	
	
	int num;
	if (version[1] == '5'){
		for (i = 0; i < ROW; i++){
			for (j = 0; j < COL; j++){
				num = fgetc(image);
				array[i][j] = num;
			}	
		}	
	}
	else{
		for (i = 0; i < ROW; i++){
			for (j = 0; j < COL; j++){
				fscanf(image, "%d ", &num);
				array[i][j] = num;
			}	
		}	
	}
	fclose(image);
}

// function to write pixel values in a 2D array to a pgm file
void writePGM(char *outName, int **output, int ROW, int COL, char *type){
	FILE *image;
	image = fopen(outName, "wb");
	
	fprintf(image, "%s", type);
	fprintf(image, "%d %d\n", COL, ROW);
	fprintf(image, "255\n");
	
	int i;
	int j;
	int num;
	
	if (!strcmp(type, "P5\n")){
		for (i = 0; i < ROW; i++){
			for (j = 0; j < COL; j++){
				num = output[i][j];
				fputc(num, image);
			}
		}	
	}
	else{
		for (i = 0; i < ROW; i++){
			for (j = 0; j < COL; j++){
				num = output[i][j];
				fprintf(image, "%d ", num);
			}
			fprintf(image, "\n");
		}	
	}
	
	fclose(image);
}


// function to implement sobel filter
void sobel(int **output, int **array, int **gx, int **gy, int ROW, int COL){
	int Mx[3][3] = {{-1, 0, 1}, {-2, 0, 2}, {-1, 0, 1}};
	int My[3][3] = {{-1, -2, -1}, {0, 0, 0}, {1, 2, 1}};
	
	int i;
	int j;
	
	for (i = 1; i < ROW - 1; i++){
		for (j = 1; j < COL - 1; j++){
			output[i][j] = newValue(i, j, Mx, My, array, gx, gy);
		}
	}
}

// function to implement min-max normalization 
void maxmin(int **output, int ROW, int COL){
	int i, j;
	int max = -9999;
	int min = 9999;
	
	for (i = 0; i < ROW; i++){
		for (j = 0; j < COL; j++){
			if (output[i][j] > max){
				max = output[i][j];
			}
			if (output[i][j] < min){
				min = output[i][j];
			}
		}
	}
	
	int gap = max - min;
	
	for (i = 0; i < ROW; i++){
		for (j = 0; j < COL; j++){
			output[i][j] = 255 * (((float) output[i][j] - (float) min)/ (float) gap);
		}
	}
}


// function to read rol and col values from a pgm files
int* rowcol(char *file){
	FILE *image;
	int ROW, COL, max;
	image = fopen(file, "rb");
	char version[3];
	
	if (image == NULL){
		printf("File cannot be opened!");
		exit(0);
	}

	fgets(version, sizeof(version), image);
	SkipComments(image);
	fscanf(image, "%d %d", &COL, &ROW);
	SkipComments(image);
	fscanf(image, "%d", &max);
	fclose(image);
    
    int *row_col = (int *)malloc(2*sizeof(int));
	row_col[0] = ROW;
	row_col[1] = COL;
	
	return row_col;
}


// function to read and return the version of a pgm file
char* filetype(char *file){
	FILE *image;
    char* version = (char *)malloc(2*sizeof(char));
	image = fopen(file, "rb");
	
	if (image == NULL){
		printf("File cannot be opened!");
		exit(0);
	}
	
	fgets(version, sizeof(version), image);
	fclose(image);
	return version;
}


int main(void){
	int i,j;
	char filename[30];
	char outputname[30];
	
	printf("Dosya adi: ");
	scanf("%s", filename);
	
    char* type = (char *)malloc(2*sizeof(char));
	type = filetype(filename);
	printf("\nPGM Type: %s", type);
	
	int *ROW_COL = (int *)malloc(sizeof(int)*2);
	ROW_COL = rowcol(filename);
	int ROW = ROW_COL[0];
	int COL = ROW_COL[1];
	printf("\nImage Size: %dx%d\n\n", COL, ROW);
	
	int *array[ROW];
	for (i = 0; i < ROW; i++)
	 	array[i] = (int *)malloc(COL*sizeof(int));
	 	
	int *output[ROW];
	for (i = 0; i < ROW; i++)
	 	output[i] = (int *)malloc(COL*sizeof(int));
	 	
	int *Gx[ROW];
	for (i = 0; i < ROW; i++)
	 	Gx[i] = (int *)malloc(COL*sizeof(int));
	int *Gy[ROW];
	for (i = 0; i < ROW; i++)
	 	Gy[i] = (int *)malloc(COL*sizeof(int));
	
	readPGM(filename, array);

	printf("Cikis dosya adi: ");
	scanf("%s", outputname);

	sobel(output, array, Gx, Gy, ROW, COL);
	
	
	maxmin(output, ROW, COL);
	maxmin(Gx, ROW, COL);
	maxmin(Gy, ROW, COL);

	for (i = 0; i < ROW; i++){
		for (j = 0; j < COL; j++){
			if (i == 0 || j == 0 || i == ROW-1 || j == COL-1){
				output[i][j] = 0;
				Gx[i][j] = 0;
				Gy[i][j] = 0;		
			}
		}
	}
	
	char name[30] = "";
	char gxname[30] = "";
	char gyname[30] = "";
	
	writePGM(outputname, output, ROW, COL, type);
		
	strncpy(name, outputname, strlen(outputname) - 4);
	strncat(gxname, name, 30);
	strncat(gxname, "_Gx.pgm", 7);
	writePGM(gxname, Gx, ROW, COL, type);
	
	strncpy(gyname, gxname, strlen(gxname) - 7);
	strncat(gyname, "_Gy.pgm", 7);
	writePGM(gyname, Gy, ROW, COL, type);
	
	for (i = 0; i < ROW; i++){
	    free(array[i]);
	}
	for (i = 0; i < ROW; i++){
	    free(output[i]);
	}
	for (i = 0; i < ROW; i++){
	    free(Gx[i]);
	}
	for (i = 0; i < ROW; i++){
	    free(Gy[i]);
	}
	return 0;
}
