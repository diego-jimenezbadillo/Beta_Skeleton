/* Get_Beta_Skeleton_2D.c */
/* 14 January 1998 */
/* Extracts the Beta Skeleton for a set of points in the 3D space */


#include <stdio.h>
#include <stdlib.h>
#include <math.h>

// C STRUCTURE TO STORE POINT COORDINATES
struct StructNode
{
   double x;
   double y;
   int id;
} *array_Nodes;

// C STRUCTURE TO STORE EDGE INFORMATION THROUGH THE INDICES OF THE
// POINTS THAT ARE LINKED
struct StructEdge
{
   int i_id;
   int j_id;
   int id;
} *RNG;

// C STRUCTURE TO STORE ATTRIBUTE INFORMATION IF NECESSARY
struct StructData
{
   double value;
   double attrib_id;
} *array_Data;

// C STRUCTURE TO STORE COORDINATES OF CIRCLE CENTRES CORRESPONDING TO
// POINT pi AND POINT pj
struct StructCentres
{
   double x_ci;
   double y_ci;

   double x_cj;
   double y_cj;
} tmp_centres;

int max_nodes; // variable to store the total quantity of points

/**************************************************/
/**************************************************/

// FUNCTION TO CALCULATE DISTANCE BETWEEN TWO POINTS IN 3D

double calc_Dist(struct StructNode point1, struct StructNode point2)
{
   double dist;
   double dist_x;
   double dist_y;
   double powdist_x;
   double powdist_y;

   dist_x = point2.x - point1.x;
   dist_y = point2.y - point1.y;

   powdist_x = pow(dist_x, 2);
   powdist_y = pow(dist_y, 2);

   dist = sqrt((powdist_x) + (powdist_y));
   return(dist);
}

/**************************************************/
/**************************************************/
/**************************************************/
/**************************************************/
/**************************************************/




void main()
{

   int i = 0;
   int j = 0;
   int k = 0;
   int p = 0;
   int q = 0;
   int r = 0;

   int max_row;
   int max_column;
   int draw_current_edge;
   int count_edges = 0;
   double **matrix;
   double dist_ij;
   double tmp_Bdist_ij;
   double tmp_dist_kci;
   double tmp_dist_kcj;
   double current_k_max;
   float Beta;
   struct StructNode tmp_ci;
   struct StructNode tmp_cj;
   char output_fileName[80];
   FILE *output_file;

   char coord_fileName[80];
   char max_points[80];

   char current_xcoordinate[80];
   char current_ycoordinate[80];

   FILE *coord_file;


/*** READ THE COORDINATE FILE ***/


   printf("Type in your file name: \n");
   scanf("%s", coord_fileName);
   coord_file = fopen(coord_fileName, "r");


   if (coord_file == NULL)
   {
      printf("Cannot open coordinate file -> %s \n", coord_fileName);
      return;
   }
   else
   {
    rewind(coord_file);
      fscanf(coord_file, "%s", &max_points);
      max_nodes = atoi(max_points);

      array_Nodes = (struct StructNode *)malloc(max_nodes * sizeof(struct StructNode));
      i = 0;
      for (i = 0; i < max_nodes; i++)
      {
         fscanf(coord_file, "%s", &current_xcoordinate);
         fscanf(coord_file, "%s", &current_ycoordinate);
       
         array_Nodes[i].x = atof(current_xcoordinate);
         array_Nodes[i].y = atof(current_ycoordinate);
         array_Nodes[i].id = i;
      }
   }
   fclose(coord_file);


/**************************************************/
/**************************************************/

/*** ALLOCATE MEMORY FOR THE MATRIX THAT WILL CONTAIN DISTANCE MEASURES ***/

   max_row = max_nodes;
   max_column = max_nodes;
   matrix = (void *)calloc(max_row, sizeof(double *));

   RNG = (struct StructEdge *)calloc(1, sizeof(struct StructEdge));

/**************************************************/
/**************************************************/

/*** ASK FOR A BETA VALUE ***/

   printf("\nEnter a Beta value greater or equal than 1: ");
   scanf("%f", &Beta);

   if(Beta < 1)
    {
         printf("ERROR: Impossible to get the Skeleton for Beta = %f\n", Beta);
         printf("\n     Beta must be a positive real number, equal or greater than 1\n\n\n");
         return;
    }

/**************************************************/
/**************************************************/

/*** COMPUTE THE BETA DISTANCE BETWEEN ALL PAIRS OF POINTS AND FILL THE MATRIX***/




   for(i=0; i<max_row; i++)
   {
      *(matrix+i) = (void *)calloc(max_column, sizeof(double));
      for(j=0; j<max_column; j++)
      {
         if(i>j)
         {
            dist_ij=calc_Dist(array_Nodes[i], array_Nodes[j]);
            if(Beta >= 1)
            {
               matrix[i][j] = (Beta * dist_ij)/2;
               matrix[j][i] = (Beta * dist_ij)/2;
            }
            else
            {
                printf("Invalid value of Beta\n");
                return;
            }
         }
         else
         {
            if(i==j)
            {
               matrix[i][j] = 0.0;
            }
         }
      }
   }

/**************************************************/
/**************************************************/

/*** FOR EACH PAIR OF POINTS (pi, pj) COMPUTE:                ***/
/*** dk_max = max{d(pk,ci), d(pk,cj)} for k=1,,,n, k<>i, k<>j ***/
count_edges = 0;
for(i = 0; i < max_nodes; i++)
{
   for(j = 0; j < max_nodes; j++)
   {
      if(i>j)
      {
         tmp_Bdist_ij = matrix[i][j];
         if(Beta >= 1)
         {
               tmp_ci.x = ((1-Beta/2) * array_Nodes[i].x) + ((Beta/2) * array_Nodes[j].x);
               tmp_ci.y = ((1-Beta/2) * array_Nodes[i].y) + ((Beta/2) * array_Nodes[j].y);            
               tmp_cj.x = ((Beta/2) * array_Nodes[i].x) + ((1-Beta/2) * array_Nodes[j].x);
               tmp_cj.y = ((Beta/2) * array_Nodes[i].y) + ((1-Beta/2) * array_Nodes[j].y);
               
         }
         else
         {
            printf("Invalid value of Beta\n");
            return;
         }
         
         // Assume that the two points are neighbours 
         draw_current_edge = 1;
 
         for (k=0; k<max_nodes; k++)
         {
            if(k != i && k != j)
            {
           // Calculate distance from point pk to point pi 
               tmp_dist_kci = calc_Dist(array_Nodes[k], tmp_ci);

               // Calculate distance from point pk to point cj
               tmp_dist_kcj = calc_Dist(array_Nodes[k], tmp_cj);

               // Determine which of the above distances is greater
               if(tmp_dist_kci > tmp_dist_kcj)
               {
                  current_k_max = tmp_dist_kci;
               }
               else
               {
                  current_k_max = tmp_dist_kcj;
               }
            }

/*** THIS IS THE TEST OF REGION EMPTINESS:
/*** FOR EACH PAIR OF POINTS (pi,pj) SEARCH FOR A VALUE
/*** dk_max THAT IS SMALLER THAN d(pi,pj) IF SUCH A POINT IS NOT FOUND
/*** THEN THE REGION IS EMPTY AND AN EDGE MUST BE DRAWN BETWEEN pi AND pj                               ***/


            if(current_k_max < tmp_Bdist_ij)
            {
               draw_current_edge = 0;
            }
         }
         
         if(draw_current_edge == 1)
         {
            RNG = (struct StructEdge *)realloc(RNG, (1 * sizeof(struct StructEdge)));

            if(RNG != NULL)
            {

                RNG[count_edges].i_id = (int)i;
                RNG[count_edges].j_id = (int)j;
                RNG[count_edges].id = (int)count_edges;
                count_edges++;
            }
            else
            {
               printf("\nMemory allocation failed!");
            }
         }
      }
   }
}

/**************************************************/
/**************************************************/
/* PRINT THE RESULT  */

   printf("Type the name of the output file: ");
   scanf("%s", output_fileName);
   output_file = fopen(output_fileName, "w");
   rewind(output_file);

   if(output_file == NULL)
   {
      printf("The output file was not opened\n");
      return;
   }
   else
   {
     fprintf(output_file, "BETA VALUE: %f\n", Beta);
     fprintf(output_file, "NUMBER OF NODES: %d\n", max_nodes);
     fprintf(output_file, "NUMBER OF EDGES: %d \n", count_edges);
     fprintf(output_file, " \nNODE COORDINATES AS FOLLOWS: \n");

     for(p=0; p<max_nodes; p++)
     {
        fprintf(output_file, "\nNode %d = %f ", p, array_Nodes[p].x);
        fprintf(output_file, "%f ", array_Nodes[p].y);
     }
     fprintf(output_file, "\n");
     fprintf(output_file, "\nEDGES AS FOLLOWS: \n");

     r = 0;
     for(r=0; r < count_edges; r++)
     {
        fprintf(output_file, "%d ", RNG[r].i_id);
        fprintf(output_file, "%d\n", RNG[r].j_id);
     }
   }


/**************************************************/
/**************************************************/
   fclose(output_file);
   free(array_Nodes);
   free(matrix);
   //free(RNG);

   printf("End of program\n");
   return;
} /* End of main function() */

