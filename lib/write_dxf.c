#include <stdio.h>
#include <stdlib.h>
#include "dxfheader.h"
#include "write_dxf.h"



// note, for unit see: https://www.autodesk.com/techpubs/autocad/acad2000/dxf/header_section_group_codes_dxf_02.htm

/* $INSUNITS
 
 70
 
 Default drawing units for AutoCAD DesignCenter blocks:
 0 = Unitless; 1 = Inches; 2 = Feet; 3 = Miles; 4 = Millimeters;
 5 = Centimeters; 6 = Meters; 7 = Kilometers; 8 = Microinches;
 9 = Mils; 10 = Yards; 11 = Angstroms; 12 = Nanometers;
 13 = Microns; 14 = Decimeters; 15 = Decameters;
 16 = Hectometers; 17 = Gigameters; 18 = Astronomical units;
 19 = Light years; 20 = Parsecs
 */

void write_dxf(const char *fname, const int **field, const int sx, const int sy) {

    // open file
    FILE *fp=fopen(fname,"w");
    
    fprintf(fp,"%s",dxf_header);
    
    double scale_z=0.4;
    double base_thickness=2.5; // in pixels (i.e. 5 mm as of writing)
    
    int n=sx,m=sy;
    
    for (int i=0; i<m; i++) {
        
        // initial vertical faces of rows
        double iz=(double)field[i][0]*scale_z;
        double iv3[3] = {-0.5,0.5+i,-0.5+iz};
        double iv4[3] = {-0.5,-0.5+i,-0.5+iz};
        double iv5[3] = {-0.5,0.5+i,(-base_thickness-0.5)};
        double iv6[3] = {-0.5,-0.5+i,(-base_thickness-0.5)};
        write_3d_face(fp,iv3,iv4,iv6,iv5);
        
        for (int j=0;j<n;j++) {
            
            int x=j, y=i;
            double z=(double)field[i][j]*scale_z;
            
            double v1[3] = {-0.5+x,-0.5+y,-0.5+z};
            double v2[3] = {-0.5+x,0.5+y,-0.5+z};
            double v3[3] = {0.5+x,0.5+y,-0.5+z};
            double v4[3] = {0.5+x,-0.5+y,-0.5+z};
            
            write_3d_face(fp,v1,v2,v3,v4);
            double z_next=(double)field[i][j+1]*scale_z;
            // connection to next face
            
            if (j<(n-1)) {
                double v5[3] = {0.5+x,0.5+y,-0.5+z_next};
                double v6[3] = {0.5+x,-0.5+y,-0.5+z_next};
                write_3d_face(fp,v3,v4,v6,v5);
            } else {
                double v5[3] = {0.5+x,0.5+y,(-base_thickness-0.5)};
                double v6[3] = {0.5+x,-0.5+y,(-base_thickness-0.5)};
                write_3d_face(fp,v3,v4,v6,v5);
            }
            
            
            // connection to previous row up
            double z_up_previous;
            if (i==0) z_up_previous = (-base_thickness-0.5);
            else z_up_previous=(double)field[i-1][j]*scale_z-0.5;
            double v7[3] = {0.5+x,-0.5+y,z_up_previous}; // below v4
            double v8[3] = {-0.5+x,-0.5+y,z_up_previous}; // below v1
            
            write_3d_face(fp,v4, v7, v8, v1);
            
            
            
        }
        
        // connection to floor down
        for (int j=0;j<n;j++) {
            double z=(double)field[m-1][j]*scale_z;
            double v2[3] = {-0.5+j,0.5+m-1,-0.5+z};
            double v3[3] = {0.5+j,0.5+m-1,-0.5+z};
            double v9[3] = {-0.5+j,0.5+m-1,(-base_thickness-0.5)}; // below v2
            double v10[3] = {0.5+j,0.5+m-1,(-base_thickness-0.5)}; // below v3
            
            write_3d_face(fp,v3, v2, v9, v10 );
        }
        int x,y;
        
        // front panel
        x=0; y=0;
        double v11[3] = {-0.5+x,-0.5+y,-0.5+field[0][0]*scale_z};
        double v12[3] = {-0.5+x,-0.5+y,(-base_thickness-0.5)};
        double v13[3] = {-0.5+x,0.5+y,(-base_thickness-0.5)};
        double v14[3] = {-0.5+x,0.5+y,-0.5+field[0][0]*scale_z};
        
        write_3d_face(fp,v11,v12,v13,v14);
        
        // back panel
        x=n-1; y=0;
        double v15[3] = {0.5+x,-0.5+y,-0.5+field[0][m-1]*scale_z};
        double v16[3] = {0.5+x,-0.5+y,(-base_thickness-0.5)};
        double v17[3] = {0.5+x,0.5+y,(-base_thickness-0.5)};
        double v18[3] ={0.5+x,0.5+y,-0.5+field[0][m-1]*scale_z};
        
        write_3d_face(fp,v15,v16,v17,v18);
    }
    
    // bottom panel
    double tl[3]={-0.5,-0.5,(-base_thickness-0.5)};
    double tr[3]={m-0.5,-0.5,(-base_thickness-0.5)};
    double br[3]={m-0.5,n-0.5,(-base_thickness-0.5)};
    double bl[3]={-0.5,n-0.5,(-base_thickness-0.5)};
    
    write_3d_face(fp,tl,tr,br,bl);
    
    // end of DXF file
    fprintf(fp, "0\n");
    fprintf(fp, "ENDSEC\n");
    fprintf(fp, "0\n");
    fprintf(fp, "EOF\n");
    
    fclose(fp);
    
}

void write_3d_face(FILE *fp, const double *av1, const double *av2, const double *av3, const double *av4) {
    
    
    double v1[3], v2[3], v3[3], v4[3]; // local copies of arguments
    
    // scale to correct size
    double scale=2.; // each pixel is 2 mm
    for (int i=0;i<3;i++) {
        
        v1[i]=av1[i]*scale;
        v2[i]=av2[i]*scale;
        v3[i]=av3[i]*scale;
        v4[i]=av4[i]*scale;
    }
    
    
    // open 3D face
    fprintf(fp, "0\n");
    fprintf(fp, "3DFACE\n");
    
    // define for layer 1
    fprintf(fp, "8\n");
    fprintf(fp, "1\n");
    
    // index color 1
    fprintf(fp, "62\n");
    fprintf(fp, "1\n");
    
    // vertex 1, x, y, z
    fprintf(fp, "10\n");
    fprintf(fp, "%f\n",v1[0]);
    fprintf(fp, "20\n");
    fprintf(fp, "%f\n",v1[1]);
    fprintf(fp, "30\n");
    fprintf(fp, "%f\n",v1[2]);
    
    // vertex 2, x, y, z
    fprintf(fp, "11\n");
    fprintf(fp, "%f\n",v2[0]);
    fprintf(fp, "21\n");
    fprintf(fp, "%f\n",v2[1]);
    fprintf(fp, "31\n");
    fprintf(fp, "%f\n",v2[2]);
    
    // vertex 3, x, y, z
    fprintf(fp, "12\n");
    fprintf(fp, "%f\n",v3[0]);
    fprintf(fp, "22\n");
    fprintf(fp, "%f\n",v3[1]);
    fprintf(fp, "32\n");
    fprintf(fp, "%f\n",v3[2]);
    
    // vertex 4, x, y, z
    fprintf(fp, "13\n");
    fprintf(fp, "%f\n",v4[0]);
    fprintf(fp, "23\n");
    fprintf(fp, "%f\n",v4[1]);
    fprintf(fp, "33\n");
    fprintf(fp, "%f\n",v4[2]);
    
    
}



