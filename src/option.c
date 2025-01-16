/**
 * @file src/options.c
 * @authors MUNAITPASOV M. & BENVENISTE A.
 * @brief Option implementation
 * @version 0.1
 * @date 2025-01-09
 *
 * @copyright licence MIT Copyright (c) 2025
 */

#include "option.h"

typedef struct option_handler
{
    char opt;
    void (*handler)(Args *__restrict__ args, char *__restrict__ optarg);
} OptionHandler;

extern void option_print_help(void);
extern int option_handle_args(int argc, char *argv[], Args *__restrict__ args);

static __inline__ bool is_double(char *__restrict__ str);
static __inline__ void init_args(Args *__restrict__ args);
static __inline__ bool is_valid_extension(const char *__restrict__ filename, const char *__restrict__ ext);
static __inline__ bool same_extensions(const char *__restrict__ input_file, const char *__restrict__ output_file);
static __inline__ void handle_h_option(Args *__restrict__ args, char *__restrict__ optarg);
static __inline__ void handle_v_option(Args *__restrict__ args, char *__restrict__ optarg);
static __inline__ void handle_g_option(Args *__restrict__ args, char *__restrict__ optarg);
static __inline__ void check_error(Args *__restrict__ args);
static bool validate_extension(Args *__restrict__ args, const char *__restrict__ optarg, bool is_input);
static void handle_c_option(Args *__restrict__ args, char *__restrict__ optarg);
static void handle_u_option(Args *__restrict__ args, char *__restrict__ optarg);
static void handle_i_option(Args *__restrict__ args, char *__restrict__ optarg);
static void handle_o_option(Args *__restrict__ args, char *__restrict__ optarg);
static void handle_a_option(Args *__restrict__ args, char *__restrict__ optarg);
static void handle_unknown_option(Args *__restrict__ args, char *__restrict__ optarg);

OptionHandler option_handlers[] = {
    {'c', handle_c_option},
    {'u', handle_u_option},
    {'h', handle_h_option},
    {'v', handle_v_option},
    {'g', handle_g_option},
    {'i', handle_i_option},
    {'o', handle_o_option},
    {'a', handle_a_option},
    {'?', handle_unknown_option},
    {0, NULL}};

static bool defined_mode = false;      /* `true` - if `encodeur` or `decodeur` is defined, false by default */
static bool defined_extension = false; /* `true` - if input file is defined, false by default */
static bool defined_output = false;    /* `true` - if output file is defined, false by default */
static bool is_valid_input = true;
static bool is_valid_output = true;

static __inline__ bool is_double(char *__restrict__ str)
{
    char *endptr;
    strtod(str, &endptr);
    return *endptr == '\0';
}

static __inline__ void init_args(Args *__restrict__ args)
{
    args->file_name_input = NULL;
    args->file_name_output = NULL;
    args->alpha = 0.0;
    args->mode = false;
    args->seg_grid = false;
    args->verbose = false;
    args->help = false;
    args->err = false;
}

extern void option_print_help(void)
{
    fprintf(stdout, "Usage: ./bin/codec [OPTIONS...]\n"
                    "`Encode / Decode` PGM (Portable Gray Map) pictures depending on options used.\n\n"
                    "First of all you need to choose between `encodeur` or `decodeur`.\n"
                    "Than program needs correct file accoring to chosen mode.\n");
    fprintf(stdout,
            "\t-h,\tdisplay help message usage and exit\n"
            "\t-v,\tverbose mode\n");
}

extern void option_print_help_verbose(void)
{
    fprintf(stdout,
            "\t-c,\tchosen mode is `encodeur` expects `.pgm` file\n"
            "\t-u,\tchosen mode is `decodeur` expects `.qtc` file\n"
            "\t-g,\tsegmentation grid\n"
            "\t-i,\tinput.{pgm | qtc}, input file depending from chosed mode\n"
            "\t-o,\toutput.{pgm | qtc}, output file depending from chosed mode\n"
            "\t-a,\t`double` in [0.0, 2.0], filtering rate for `encodeur`\n");
}

static __inline__ bool is_valid_extension(
    const char *__restrict__ filename,
    const char *__restrict__ ext)
{
    size_t len = 0UL;
    if (!filename || !ext)
    {
        return false;
    }
    len = strlen(filename);
    if (len < 5UL) /* length of files with ".pgm" or ".qtc" extensions */
    {
        return false;
    }
    return !strcmp(filename + len - 4, ext);
}

static __inline__ bool same_extensions(
    const char *__restrict__ input_file,
    const char *__restrict__ output_file)
{
    size_t len_input = 0UL;
    size_t len_output = 0UL;
    if (!input_file || !output_file)
    {
        return false;
    }
    len_input = strlen(input_file);
    len_output = strlen(output_file);
    if (len_input < 5UL || len_output < 5UL) /* length of files with ".pgm" or ".qtc" extensions */
    {
        return false;
    }
    return !strcmp(input_file + (len_input - 4),
                   output_file + (len_output - 4));
}

static void handle_c_option(Args *__restrict__ args, char *__restrict__ optarg)
{
    if (!args)
    {
        (void)optarg;
        return;
    }
    if (defined_mode)
    {
        fprintf(stderr, "Double mode error: `encodeur` is already defined\n");
        args->err = true;
        return;
    }
    args->mode = false;
    defined_mode = true;
}

static void handle_u_option(Args *__restrict__ args, char *__restrict__ optarg)
{
    if (!args)
    {
        (void)optarg;
        return;
    }
    if (defined_mode)
    {
        fprintf(stderr, "Double mode error: `decodeur` is already defined\n");
        args->err = true;
        return;
    }
    args->mode = true;
    defined_mode = true;
}

static __inline__ void handle_h_option(Args *__restrict__ args, char *__restrict__ optarg)
{
    if (!args)
    {
        (void)optarg;
        return;
    }
    args->help = true;
}

static __inline__ void handle_v_option(Args *__restrict__ args, char *__restrict__ optarg)
{
    if (!args)
    {
        (void)optarg;
        return;
    }
    args->verbose = true;
}

static __inline__ void handle_g_option(Args *__restrict__ args, char *__restrict__ optarg)
{
    if (!args)
    {
        (void)optarg;
        return;
    }
    args->seg_grid = true;
}

static __inline__ void check_error(Args *__restrict__ args)
{
    bool is_identical = false;
    if (!args)
    {
        return;
    }
    if (!args->file_name_input)
    {
        fprintf(stderr, "Error: no input.{pgm | qtc} file\n");
        args->err = true;
        return;
    }

    if (defined_mode && defined_extension)
    {
        if (!args->mode && !is_valid_extension(args->file_name_input, ".pgm"))
        {
            fprintf(stderr, "Error: mode `encodeur` is only for `*.pgm` files\n");
            args->err = true;
            is_valid_input = false;
            return;
        }
        else if (args->mode && !is_valid_extension(args->file_name_input, ".qtc"))
        {
            fprintf(stderr, "Error: mode `decodeur` is only for `*.qtc` files\n");
            args->err = true;
            is_valid_input = false;
            return;
        }
    }
    is_identical = same_extensions(args->file_name_input, args->file_name_output);
    /* file output must have different extension than input file */
    if (args->file_name_output && is_identical)
    {
        fprintf(stderr, "Error: input and output files extension must have different extension\n");
        args->err = true;
        is_valid_output = false;
        return;
    }
    if (!defined_mode)
    {
        fprintf(stderr, "Error: mode `encodeur` or `decodeur` is not defined\n");
        args->err = true;
        return;
    }
}

static bool validate_extension(Args *__restrict__ args, const char *__restrict__ optarg, bool is_input)
{
    const char *expected_input_extension = args->mode ? ".qtc" : ".pgm";
    const char *expected_output_extension = args->mode ? ".pgm" : ".qtc";
    const char *file_type = is_input ? "input" : "output";

    if (!optarg)
    {
        return false;
    }

    if (is_input)
    {
        if (defined_mode && !is_valid_extension(optarg, expected_input_extension))
        {
            fprintf(stderr, "Error: Input file for `%s` is only allowed with `%s` extension\n",
                    (args->mode ? "decodeur" : "encodeur"), expected_input_extension);
            args->err = true;
            is_valid_input = false;
            return false;
        }
    }
    else
    {
        if (defined_mode && !is_valid_extension(optarg, expected_output_extension))
        {
            fprintf(stderr, "Error: Output file is only allowed with `%s` extension\n", expected_output_extension);
            args->err = true;
            is_valid_output = false;
            return false;
        }
    }

    if (!is_valid_extension(optarg, ".pgm") &&
        !is_valid_extension(optarg, ".qtc"))
    {
        fprintf(stderr, "Error: %s file extension - `%s` is not correct\n", file_type, optarg);
        args->err = true;
        if (is_input)
        {
            is_valid_input = false;
        }
        else
        {
            is_valid_output = false;
        }
        return false;
    }

    defined_extension = true;
    return true;
}

static void handle_i_option(Args *__restrict__ args, char *__restrict__ optarg)
{
    if (!args || !optarg)
    {
        return;
    }
    if (validate_extension(args, optarg, true))
    {
        args->file_name_input = optarg;
    }
}

static void handle_o_option(Args *__restrict__ args, char *__restrict__ optarg)
{
    if (!args || !optarg)
    {
        return;
    }
    if (validate_extension(args, optarg, false))
    {
        defined_output = true;
        args->file_name_output = optarg;
    }
}

static void handle_a_option(Args *__restrict__ args, char *__restrict__ optarg)
{
    if (!args || !optarg)
    {
        return;
    }
    if (!is_double(optarg))
    {
        fprintf(stderr, "Error: alpha value must be a double\n");
        args->err = true;
        return;
    }
    args->alpha = atof(optarg);
    if (args->alpha < 0.0 || args->alpha > 2.0)
    {
        fprintf(stderr, "Error: alpha value must be between 0.0 and 2.0\n");
        args->err = true;
    }
}

static void handle_unknown_option(Args *__restrict__ args, char *__restrict__ optarg)
{
    if (!args || !optarg)
    {
        return;
    }
    fprintf(stderr, "Unknown option : %c\n", optopt);
    args->err = true;
}

extern int option_handle_args(int argc, char *argv[], Args *__restrict__ args)
{
    int opt = 0;
    OptionHandler *handler = NULL;
    init_args(args);

    while ((opt = getopt(argc, argv, "cuhvgi:o:a:")) != -1)
    {
        for (handler = option_handlers; handler->opt != 0; ++handler)
        {
            if (handler->opt == opt)
            {
                handler->handler(args, optarg);
                break;
            }
        }
    }

    if (optind < argc && !args->file_name_input)
    {
        if (!is_valid_extension(argv[optind], ".pgm") &&
            !is_valid_extension(argv[optind], ".qtc"))
        {
            is_valid_input = false;
            fprintf(stderr, "Error: input file - `%s` is not correct\n", argv[optind]);
            args->err = true;
            return args->err;
        }
        args->file_name_input = argv[optind];
        defined_extension = true;
    }
    if (!defined_output && defined_mode && defined_extension)
    {
        args->file_name_output = args->mode ? "PGM/out.pgm" : "QTC/out.qtc";
    }
    check_error(args);

    return args->err;
}
