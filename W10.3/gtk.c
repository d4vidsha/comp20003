/* Compile with
gcc `pkg-config --cflags gtk+-3.0` -o gtk gtk.c `pkg-config --libs gtk+-3.0` -lm -g `pkg-config --static --libs --cflags igraph`
*/

#include <gtk/gtk.h>
#include <math.h>
#include <cairo.h>
#include <igraph.h>
#include <assert.h>
#include <stdlib.h>
#include "gtk.h"

#define WIDTH   640
#define HEIGHT  480

#define ZOOM_X  100.0
#define ZOOM_Y  100.0

// #define NUMVERTICES 6
// #define NUMEDGES 7
#define LAYOUTDIMENSIONS 2

/* Distance along arrow to label weight. */
#define LABEL_DISTANCE (0.5)
#define FONT_SIZE (0.1)
#define Y_OFFSET (0.05)
#define X_OFFSET (0.05)

#define VERTEX_FONT_SIZE (0.1)

gfloat f (gfloat x)
{
    return 0.03 * pow (x, 3);
}

igraph_t graph;
igraph_vector_int_t edges;
// Edges in shortest path 0 excluded, 1 included.
igraph_vector_t shortest_path;
igraph_matrix_t res;

gdouble minx;
gdouble miny;
gdouble maxx;
gdouble maxy;

int srcSelected = 1;
int dstSelected = -1;

double pointer_x = 0;
double pointer_y = 0;

int gtkEdgeCount = 0;
int gtkNodeCount = 0;

void **dijkstraResults = NULL;

// GtkWidget **weightLabels = NULL;
char **weightStrings = NULL;
char **gtkVertexLabels = NULL;

/* Get the dijkstra result for the given source node. */
void *fetchDijkstraRes(int srcSelected);

void setupGTKgraph(int nodeCount, int edgeCount, int *sources, int *destinations, int *weights, char **vertexLabels){
    int i;
    gtkEdgeCount = nodeCount;
    gtkNodeCount = edgeCount;
    /* Set vertex labels. */
    gtkVertexLabels = vertexLabels;
    /* Create weight labels */
    // weightLabels = (GtkWidget **) malloc(sizeof(GtkWidget *) * edgeCount);
    // assert(weightLabels || edgeCount == 0);
    weightStrings = (char **) malloc(sizeof(char **) * edgeCount);
    assert(weightStrings || edgeCount == 0);
    for(i = 0; i < edgeCount; i++){
        weightStrings[i] = (char *) malloc((sizeof(int) * 8 + 1) * sizeof(char));
        assert(weightStrings[i]);
        /* Base 10 */
        sprintf(weightStrings[i], "%d", weights[i]);
        // weightLabels[i] = gtk_label_new(weightStrings[i]);
    }
    
    
    /*****************************************/
    /*            igraph stuff               */
    /*****************************************/
    igraph_vector_int_init(&edges, 2*edgeCount);
    /* Edges source -> destination */
    for(i = 0; i < edgeCount; i++){
        VECTOR(edges)[i * 2] = sources[i]; 
        VECTOR(edges)[i * 2 + 1] = destinations[i];
    }
    
    igraph_create(&graph, &edges, 0, 1);
    
    // X, Y coordinates for each vertex
    igraph_matrix_init(&res, nodeCount, LAYOUTDIMENSIONS);
    
    // igraph_layout_circle(&graph, &res);
    // graphopt constants from https://igraph.org/c/doc/igraph-Layout.html#igraph_layout_graphopt
    igraph_layout_graphopt(&graph, &res, 500, 0.001, 30, 0, 1, 5, 0);
    
    // Rescale into bounds [[-1, 1], [-1, 1]]
    long int j, k;
    
    for(j = 0; j < gtkNodeCount; j++){
        for(k = 0; k < LAYOUTDIMENSIONS; k++){
            if(j == 0){
                if(k == 0){
                    minx = igraph_matrix_get(&res, j, k);
                    maxx = igraph_matrix_get(&res, j, k);
                } else {
                    miny = igraph_matrix_get(&res, j, k);
                    maxy = igraph_matrix_get(&res, j, k);
                }
            } else {
                if(k == 0){
                    if(igraph_matrix_get(&res, j, k) < minx){
                        minx = igraph_matrix_get(&res, j, k);
                    }
                    if(igraph_matrix_get(&res, j, k) > maxx){
                        maxx = igraph_matrix_get(&res, j, k);
                    }
                } else {
                    if(igraph_matrix_get(&res, j, k) < miny){
                        miny = igraph_matrix_get(&res, j, k);
                    }
                    if(igraph_matrix_get(&res, j, k) > maxy){
                        maxy = igraph_matrix_get(&res, j, k);
                    }
                }
            }
            
            // printf("%f ", igraph_matrix_get(&res, j, k));
        }
        // printf("\n");
    }
    if (minx == maxx){
        minx = (-1);
        maxx = 1;
    }
    if (miny == maxy){
        miny = (-1);
        maxy = 1;
    }
    /* Resizing */
    for(j = 0; j < gtkNodeCount; j++){
        for(k = 0; k < LAYOUTDIMENSIONS; k++){
            
            if(k == 0){
                igraph_matrix_set(&res, j, k, (igraph_matrix_get(&res, j, k) - minx) / (maxx - minx) * 2 - 1);
            } else {
                igraph_matrix_set(&res, j, k, (igraph_matrix_get(&res, j, k) - miny) / (maxy - miny) * 2 - 1);
            }
            // printf("%f ", igraph_matrix_get(&res, j, k));
        }
        // printf("\n");
    }
    
    /* Set up shortest paths. */
    igraph_vector_init(&shortest_path, edgeCount);
    for(j = 0; j < edgeCount; j++){
        VECTOR(shortest_path)[j] = 0;
    }
}

void *(*gtkDijkstraRun)(void *graph, int source);
void (*gtkConstructPath)(void *res, void *graph, int *path, int destination);
void (*gtkFreeDijkstraRes)(void *res);
void *gtkDigraph = NULL;

void setupGTKdijkstra(void *digraph, 
    void *(*dijkstra)(void *graph, int source), 
    void (*constructPath)(void *res, void *graph, int *path, 
    int destination), 
    void (*freeDijkstraRes)(void *res)){
    
    gtkDigraph = digraph;
    gtkDijkstraRun = dijkstra;
    gtkConstructPath = constructPath;
}

void *fetchDijkstraRes(int srcSelected){
    int i;
    if (!dijkstraResults){
        dijkstraResults = (void **) malloc(sizeof(void *) * gtkNodeCount);
        assert(dijkstraResults);
        for(i = 0; i < gtkNodeCount; i++){
            dijkstraResults[i] = NULL;
        }
    }
    if(dijkstraResults[srcSelected]){
        return dijkstraResults[srcSelected];
    } else {
        dijkstraResults[srcSelected] = gtkDijkstraRun(gtkDigraph, srcSelected);
        return dijkstraResults[srcSelected];
    }
}

void update_destination(){
    int closest_v = 0;
    int i;
    double d2;
    double closest_d2;
    double temp_dx, temp_dy;
    for(i = 0; i < gtkNodeCount; i++){
        if (i == 0){
            temp_dx = igraph_matrix_get(&res, closest_v, 0) - pointer_x;
            temp_dy = igraph_matrix_get(&res, closest_v, 1) - pointer_y;
            closest_d2 = temp_dx * temp_dx + temp_dy * temp_dy;
        } else {
            temp_dx = igraph_matrix_get(&res, i, 0) - pointer_x;
            temp_dy = igraph_matrix_get(&res, i, 1) - pointer_y;
            d2 = temp_dx * temp_dx + temp_dy * temp_dy;
            if(d2 < closest_d2){
                closest_d2 = d2;
                closest_v = i;
            }
        }
    }
    if (closest_d2 < (0.01)){
        if (closest_v != dstSelected){
            dstSelected = closest_v;
            printf("%d to %d\n", srcSelected, dstSelected);
            int path[gtkEdgeCount];
            for(i = 0; i < gtkEdgeCount; i++){
                path[i] = 0;
            }
            gtkConstructPath(fetchDijkstraRes(srcSelected), gtkDigraph, path, dstSelected);
            for(i = 0; i < gtkEdgeCount; i++){
                VECTOR(shortest_path)[i] = path[i];
            }
        }
    } else {
        if (dstSelected != (-1)){
            dstSelected = -1;
            printf("%d to %d\n", srcSelected, dstSelected);
            for(i = 0; i < gtkEdgeCount; i++){
                VECTOR(shortest_path)[i] = 0;
            }
        }
    }
}

static gboolean
on_draw (GtkWidget *widget, cairo_t *cr, gpointer user_data)
{
    GdkRectangle da;            /* GtkDrawingArea size */
    gdouble dx = 5.0, dy = 5.0; /* Pixels between each point */
    gdouble clip_x1 = 0.0, clip_y1 = 0.0, clip_x2 = 0.0, clip_y2 = 0.0;
    GdkWindow *window = gtk_widget_get_window(widget);

    /* Determine GtkDrawingArea dimensions */
    gdk_window_get_geometry (window,
            &da.x,
            &da.y,
            &da.width,
            &da.height);
    
    /***************************************/

    /* Draw on a black background */
    cairo_set_source_rgb (cr, 0.0, 0.0, 0.0);
    cairo_paint (cr);

    /* Change the transformation matrix */
    cairo_translate (cr, da.width / 2, da.height / 2);
    cairo_scale (cr, ZOOM_X, ZOOM_Y);

    /* Determine the data points to calculate (ie. those in the clipping zone) */
    cairo_device_to_user_distance (cr, &dx, &dy);
    cairo_clip_extents (cr, &clip_x1, &clip_y1, &clip_x2, &clip_y2);
    cairo_set_line_width (cr, dx);

    /* Draw points */
    double x, y;
    int j;
    for(j = 0; j < gtkNodeCount; j++){
        x = igraph_matrix_get(&res, j, 0);
        y = igraph_matrix_get(&res, j, 1);
        cairo_move_to (cr, x, y);
        cairo_new_sub_path(cr);
        // Draw selected source in yellow.
        if (j == srcSelected){
            cairo_set_source_rgba (cr, 1, 0.6, 0.2, 0.6);
        } else if (j == dstSelected) {
            cairo_set_source_rgba (cr, 1, 0.2, 1.0, 0.6);
        } else {
            cairo_set_source_rgba (cr, 1, 0.2, 0.2, 0.6);
        }
        
        cairo_arc(cr, x, y, 0.1, 0, 2 * M_PI);
        cairo_stroke(cr);
        
        if(gtkVertexLabels && gtkVertexLabels[j]){
            if (j == srcSelected){
                cairo_set_source_rgba (cr, 1, 0.6, 0.2, 0.6);
            } else if (j == dstSelected) {
                cairo_set_source_rgba (cr, 1, 0.2, 1.0, 0.6);
            } else {
                cairo_set_source_rgba (cr, 1, 0.2, 0.2, 0.6);
            }
            
            cairo_set_font_size(cr, VERTEX_FONT_SIZE);
            /* Get extents so text can be centred */
            cairo_text_extents_t offsets;
            cairo_text_extents(cr, gtkVertexLabels[j], &offsets);
            cairo_move_to (cr, x - offsets.x_advance / 2.0, y + offsets.height / 2.0);
            
            cairo_show_text(cr, gtkVertexLabels[j]);
        }
    }
    
    /* Draw edges */
    igraph_es_t es;
    igraph_eit_t eit;
    igraph_integer_t from;
    igraph_integer_t to;
    double x_from;
    double y_from;
    double x_to;
    double y_to;
    for(j = 0; j < gtkNodeCount; j++){
        int edges_left = 1;
        igraph_es_incident(&es, j, IGRAPH_OUT);
        igraph_eit_create(&graph, es, &eit);
        // printf("%d\n", j);
        if(IGRAPH_EIT_SIZE(eit) == 0){
            edges_left = 0;
        }
        while(edges_left) {
            igraph_edge(&graph, IGRAPH_EIT_GET(eit), &from, &to);
            x_from = igraph_matrix_get(&res, from, 0);
            y_from = igraph_matrix_get(&res, from, 1);
            x_to = igraph_matrix_get(&res, to, 0);
            y_to = igraph_matrix_get(&res, to, 1);
            // printf("%f %f (%d) -> %f %f (%d)\n", 
            //        igraph_matrix_get(&res, from, 0),
            //        igraph_matrix_get(&res, from, 1),
            //        from,
            //        igraph_matrix_get(&res, to, 0),
            //        igraph_matrix_get(&res, to, 1),
            //        to
            //       );
            // Draw edges
            cairo_move_to (cr, (x_from), (y_from));
            if (VECTOR(shortest_path)[IGRAPH_EIT_GET(eit)]){
                // Included in shortest path.
                cairo_set_source_rgba (cr, 1, 1, 1, 0.7);
            } else {
                cairo_set_source_rgba (cr, 0.2, 0.2, 1, 0.6);
            }
            // Draw arrowhead code from http://kapo-cpp.blogspot.com/2008/10/drawing-arrows-with-cairo.html
            double angle = atan2 (y_to - y_from, x_to - x_from) + M_PI;
            // 0.05 shorter to avoid intersecting the circles
            cairo_line_to (cr, (x_to + 0.05 * cos(angle)), (y_to + 0.05 * sin(angle)));
            cairo_stroke(cr);
            
            double arrow_degrees = 0.5;
            double x1 = x_to + 0.05 * cos(angle) + 0.2 * cos(angle - arrow_degrees);
            double y1 = y_to + 0.05 * sin(angle) + 0.2 * sin(angle - arrow_degrees);
            double x2 = x_to + 0.05 * cos(angle) + 0.2 * cos(angle + arrow_degrees);
            double y2 = y_to + 0.05 * sin(angle) + 0.2 * sin(angle + arrow_degrees);
            cairo_move_to (cr, x_to + 0.05 * cos(angle), y_to + 0.05 * sin(angle));
            cairo_line_to (cr, x1, y1);
            cairo_stroke(cr);

            cairo_move_to (cr, x_to + 0.05 * cos(angle), y_to + 0.05 * sin(angle));
            cairo_line_to (cr, x2, y2);
            cairo_stroke(cr);
            
            // Set text to fully visible
            if (VECTOR(shortest_path)[IGRAPH_EIT_GET(eit)]){
                // Included in shortest path.
                cairo_set_source_rgba (cr, 1, 1, 1, 1);
            } else {
                cairo_set_source_rgba (cr, 0.2, 0.2, 1, 1);
            }
            
            cairo_move_to (cr, x_from + (LABEL_DISTANCE) * (x_to - x_from) + X_OFFSET,
                           y_from + (LABEL_DISTANCE) * (y_to - y_from) + Y_OFFSET);
            cairo_set_font_size(cr, FONT_SIZE);
            cairo_show_text(cr, weightStrings[IGRAPH_EIT_GET(eit)]);
            
            if(! IGRAPH_EIT_END(eit)){
                IGRAPH_EIT_NEXT(eit);
                if(IGRAPH_EIT_END(eit)){
                    edges_left = 0;
                }
            } else {
                edges_left = 0;
            }
        }
    }
    
    /* Draw cursor circle */
    cairo_move_to (cr, pointer_x, pointer_y);
    cairo_new_sub_path(cr);
    cairo_set_source_rgba (cr, 0.2, 0.2, 0.2, 0.6);
    cairo_arc(cr, pointer_x, pointer_y, 0.1, 0, M_PI / 2);
    cairo_stroke(cr);
    cairo_arc(cr, pointer_x, pointer_y, 0.1, M_PI, 3 * M_PI / 2);
    cairo_stroke(cr);

    return FALSE;
}

static gboolean mouse_moved(GtkWidget *widget, GdkEvent *event, gpointer user_data) {

    if (event->type==GDK_MOTION_NOTIFY) {
        GdkEventMotion* e=(GdkEventMotion*)event;
        // printf("Coordinates: (%f,%f)\n", e->x, e->y);
        GdkRectangle da;            /* GtkDrawingArea size */
        GdkWindow *window = gtk_widget_get_window(widget);

        /* Determine GtkDrawingArea dimensions */
        gdk_window_get_geometry (window,
                &da.x,
                &da.y,
                &da.width,
                &da.height);
        // printf("%f %f %d %d\n", (e->x - da.width / 2) / dx, (e->y - da.height / 2) / dy, da.width, da.height);
        // Scale to Cairo zoom level
        pointer_x = (e->x - da.width / 2) / ZOOM_X;
        pointer_y = (e->y - da.height / 2) / (ZOOM_Y);
        update_destination();
        
        gtk_widget_queue_draw(widget);
    }
    return FALSE;
}

static gboolean button_pressed(GtkWidget *widget, GdkEvent *event, gpointer user_data){
    if(dstSelected != -1 && dstSelected != srcSelected){
        srcSelected = dstSelected;
        update_destination();
        printf("Changed source vertex to %d\n", srcSelected);
    }
    
    return FALSE;
}

GtkWidget *window = NULL;
GtkWidget *da = NULL;

void setupGTK(int *argc, char ***argv, char *title){
    gtk_init (argc, argv);
    window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_default_size(GTK_WINDOW (window), WIDTH, HEIGHT);
    gtk_window_set_title(GTK_WINDOW (window), title);

    g_signal_connect(G_OBJECT (window), "destroy", gtk_main_quit, NULL);

    da = gtk_drawing_area_new();

    gtk_container_add(GTK_CONTAINER(window), da);
}

void runGTKInteraction(){
    g_signal_connect (G_OBJECT (da),
            "draw",
            G_CALLBACK (on_draw),
            NULL);
    
    g_signal_connect (G_OBJECT (da),
            "motion-notify-event",
            G_CALLBACK (mouse_moved),
            NULL);

    g_signal_connect (G_OBJECT (window),
            "button-release-event",
            G_CALLBACK (button_pressed),
            NULL);
    
    gtk_widget_set_events(da, GDK_POINTER_MOTION_MASK);
    gtk_widget_add_events(window, GDK_BUTTON_RELEASE_MASK);
    gtk_widget_show_all(window);
    gtk_main();
}

void freeGTK(){

}