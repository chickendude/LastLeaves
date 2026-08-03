#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "global.h"
#include "battle_vars.h"

const char output_filename_c[] = "../src/data/party_combos.c";
const char output_filename_h[] = "../src/data/party_combos.h";

typedef struct ComboData
{
    char name[20];
    char variable_name[30];
    AttackDir attacks[10];
} ComboData;

char *filenames[MAX_PARTY_SIZE] = {
    "../data/attacks_lynne.txt",
    "../data/attacks_roak.txt",
    "../data/attacks_tann.txt",
};

char buffer[1024 * 5];

// Combos by player, 3 players with up to 15 combos each
ComboData combos[3][15];

int read_buffer(FILE *f, int player_index);

void convert_to_variable_name(char *attack_name, char *variable_name);

void write_combos(FILE *f, const ComboData *combo);

int main()
{
    for (int i = 0; i < MAX_PARTY_SIZE; i++)
    {
        const char *filename = filenames[i];
        FILE *f = fopen(filename, "r");

        fseek(f, 0L, SEEK_END);
        const long filesize = ftell(f);
        fseek(f, 0L, SEEK_SET);
        if (read_buffer(f, i))
        {
            printf("\nError parsing text.\n");
            return 0;
        }

        fclose(f);
    }

    FILE *file_c = fopen(output_filename_c, "w");
    FILE *file_h = fopen(output_filename_h, "w");
    fprintf(
        file_c,
        "// This was generated from the 'attacks_[...].txt' "
        "files in the data folder\n// by running tools/build_party\n\n"
        "#include \"party_combos.h\"\n\n"
    );
    fprintf(
        file_h,
        "#ifndef LASTLEAVES_PARTY_COMBOS_H\n#define LASTLEAVES_PARTY_COMBOS_H\n\n"
        "#include \"../battle_vars.h\"\n\n"
    );

    // Loop through each character's combos
    for (int i = 0; i < MAX_PARTY_SIZE; i++)
    {
        int combo_count = 0;
        // Create copy of filename to work with
        char input_filename[40];
        strcpy(input_filename, filenames[i]);

        // Strip out the front of string before the character's name
        const char *character_name = strchr(input_filename, '_') + 1;

        // Remove file extension
        strchr(character_name, '.')[0] = 0;

        // Add comment in file to mark start of character's moves
        fprintf(file_c, "// %s\n\n", character_name);
        printf("\n");
        for (int j = 0; j < 10; j++)
        {
            ComboData *combo = &combos[i][j];
            if (combo->name[0] == '\0') continue;
            combo_count++;
            fprintf(file_h, "extern const Combo combo_%s_%s;\n", character_name,
                    combo->variable_name);
            fprintf(file_c, "const Combo combo_%s_%s = {\n", character_name,
                    combo->variable_name);
            fprintf(file_c, "    .name = \"%s\",\n", combo->name);
            fprintf(file_c, "    .animation = {},\n");
            fprintf(file_c, "    .combo = {");
            write_combos(file_c, combo);
            fprintf(file_c, "}\n");
            fprintf(file_c, "};\n\n");
        }

        // Write full combo array
        fprintf(file_h, "\nextern const Combo combos_%s[%d];\n\n", character_name, combo_count);
        fprintf(file_c, "const Combo combos_%s[%d] = { ", character_name, combo_count);
        for (int j = 0; j < 10; j++)
        {
            ComboData *combo = &combos[i][j];
            if (combo->name[0] == '\0') continue;
            fprintf(file_c, "combo_%s_%s, ", character_name,
                    combo->variable_name);
        }
        fprintf(file_c, "};\n\n");
    }
    fprintf(file_h, "#endif //LASTLEAVES_PARTY_COMBOS_H");
    fclose(file_h);
    fclose(file_c);
}

void write_combos(FILE *f, const ComboData *combo)
{
    for (int i = 0; i < 10; i++)
    {
        const AttackDir dir = combo->attacks[i];
        if (dir == ATK_HIGH) fprintf(f, "ATK_HIGH");
        else if (dir == ATK_LOW) fprintf(f, "ATK_LOW");
        else if (dir == ATK_LEFT) fprintf(f, "ATK_LEFT");
        else if (dir == ATK_RIGHT) fprintf(f, "ATK_RIGHT");
        if (i < 9 && combo->attacks[i + 1] != ATK_NONE) fprintf(f, ", ");
    }
}

int read_buffer(FILE *f, int player_index)
{
    char line[256];
    int combo_index = 0;
    while (fgets(line, 256, f))
    {
        if (line[0] == ';')
        {
            printf("[Skipping comment]\n");
            continue;
        }

        // Make sure correct num of commas
        int num_commas = 0;
        int comma_index = 0;
        for (int i = 0; i < strlen(line); i++)
        {
            if (line[i] == ',')
            {
                num_commas++;
                comma_index = i;
            }
        }
        if (num_commas != 1) return 1;

        // Save move set
        char *moveset = &line[comma_index + 1];

        // Zero terminate move name and trim spaces
        line[comma_index] = '\0';
        while (line[comma_index - 1] == ' ')
        {
            line[--comma_index] = '\0';
        }
        line[comma_index] = '\0';

        strcpy(combos[player_index][combo_index].name, line);

        // Convert string to ATK
        int attack_index = 0;
        for (int i = 0; i < strlen(moveset); i++)
        {
            switch (moveset[i])
            {
                case 'u':
                case 'U':
                    combos[player_index][combo_index].attacks[attack_index++] =
                            ATK_HIGH;
                    break;
                case 'd':
                case 'D':
                    combos[player_index][combo_index].attacks[attack_index++] =
                            ATK_LOW;
                    break;
                case 'r':
                case 'R':
                    combos[player_index][combo_index].attacks[attack_index++] =
                            ATK_RIGHT;
                    break;
                case 'l':
                case 'L':
                    combos[player_index][combo_index].attacks[attack_index++] =
                            ATK_LEFT;
                    break;
                default:
                    break;
            }
        }
        convert_to_variable_name(combos[player_index][combo_index].name,
                                 combos[player_index][combo_index].
                                 variable_name);
        combos[player_index][combo_index].attacks[attack_index] = ATK_NONE;
        combo_index++;
    }
    return 0;
}

#undef tolower
void convert_to_variable_name(char *attack_name, char *variable_name)
{
    for (int i = 0; attack_name[i]; i++)
    {
        const char letter = (char) tolower(attack_name[i]);
        variable_name[i] = letter == ' ' ? '_' : letter;
    }
}
