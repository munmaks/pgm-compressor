/**
 * @file src/bits_operations.c
 * @authors MUNAITPASOV M. & BENVENISTE A.
 * @brief Bit operations implementation
 * @version 0.1
 * @date 2025-01-09
 *
 * @copyright licence MIT Copyright (c) 2025
 *
 */

#include "qtc.h"
/* #include "grid.h" */

int from_pgm_to_qtc(Args *args, Pixmap *pix, QTree *tree)
{
    Pixmap grid = {0};
    char *seg_grid_file = NULL;
    unsigned char level = 0;

    init_pixmap(pix, args->file_name_input);

    level = determine_qtree_level(pix);

    (void)make_qtree(tree, pix->grey_level, level);

    init_quadtree(tree, pix);

    if (args->alpha >= 0.1) /* filtering */
    {
        must_filter_qtree(tree, args->alpha, true);
    }

    if (args->seg_grid)
    {
        generate_grid_from_quadtree(tree, &grid);

        seg_grid_file = change_filename_to_seg_grid(args->file_name_input);
        from_pixmap_to_pgm(&grid, seg_grid_file);
        free(seg_grid_file);

        free_pixmap(&grid);
    }

    create_qtc_file(tree, pix->width, args->file_name_output);

    free_pixmap(pix);
    free_qtree(tree);
    return 0;
}

int from_qtc_to_pgm(Args *args, Pixmap *pix, QTree *tree)
{
    Pixmap grid = {0};
    char *seg_grid_file = NULL;
    init_quadtree_from_file(tree, args->file_name_input);

    pixmap_from_quadtree(tree, pix);

    if (args->seg_grid)
    {
        generate_grid_from_quadtree(tree, &grid);

        seg_grid_file = change_filename_to_seg_grid(args->file_name_output);

        from_pixmap_to_pgm(&grid, seg_grid_file);

        free(seg_grid_file); /* free the allocated memory */

        free_pixmap(&grid);
    }
    from_pixmap_to_pgm(pix, args->file_name_output);

    free_pixmap(pix);
    free_qtree(tree);

    return 0;
}

int main(int argc, char *argv[])
{
    Args args = {0};
    Pixmap pix = {0};
    QTree tree = {0};

    option_handle_args(argc, argv, &args);

    if (args.help)
    {
        option_print_help();
        if (args.verbose)
        {
            option_print_help_verbose();
        }
        return 0;
    }
    else if (args.err) /* error in arguments */
    {
        return 1;
    }
    if (!args.mode) /* PGM to QTC: encode */
    {
        return from_pgm_to_qtc(&args, &pix, &tree);
    }
    else /* QTC to PGM: decode */
    {
        return from_qtc_to_pgm(&args, &pix, &tree);
    }
}
