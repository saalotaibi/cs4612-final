#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <time.h>
#include <limits.h>

// هيكل عشان نخزن إحصائيات الجينوم
typedef struct {
    long long total_bases;
    long long gc_count;
    long long at_count;
    long long n_count;
    long long other_count;
    int sequence_count;
    long long max_sequence_length;
    long long min_sequence_length;
    long long total_sequence_length;
} GenomeStats;

// دالة عشان نعالج قاعدة واحدة ونحدث الإحصائيات
void process_base(char base, GenomeStats *stats) {
    stats->total_bases++;

    // نحول للحروف الكبيرة عشان نضمن التناسق
    base = toupper(base);

    switch(base) {
        case 'G':
        case 'C':
            stats->gc_count++;
            break;
        case 'A':
        case 'T':
            stats->at_count++;
            break;
        case 'N':
            stats->n_count++;
            break;
        default:
            stats->other_count++;
            break;
    }
}

// دالة عشان نقرأ ملف FASTA ونحسب الإحصائيات
GenomeStats process_fasta_file(const char *filename) {
    GenomeStats stats = {0};
    stats.min_sequence_length = LLONG_MAX;

    FILE *fp = fopen(filename, "r");
    if (!fp) {
        fprintf(stderr, "Error: Cannot open file %s\n", filename);
        exit(1);
    }

    char line[4096];
    int in_sequence = 0;
    long long current_sequence_length = 0;

    while (fgets(line, sizeof(line), fp)) {
        // نشيل رمز السطر الجديد
        line[strcspn(line, "\n")] = 0;

        if (line[0] == '>') {
            // هذا سطر عنوان
            if (in_sequence && current_sequence_length > 0) {
                // نحدث الإحصائيات للسلسلة السابقة
                if (current_sequence_length > stats.max_sequence_length) {
                    stats.max_sequence_length = current_sequence_length;
                }
                if (current_sequence_length < stats.min_sequence_length) {
                    stats.min_sequence_length = current_sequence_length;
                }
                stats.total_sequence_length += current_sequence_length;
            }

            stats.sequence_count++;
            in_sequence = 1;
            current_sequence_length = 0;
        } else if (in_sequence && strlen(line) > 0) {
            // هذا سطر سلسلة
            for (size_t i = 0; i < strlen(line); i++) {
                process_base(line[i], &stats);
                current_sequence_length++;
            }
        }
    }

    // نتعامل مع السلسلة الأخيرة
    if (in_sequence && current_sequence_length > 0) {
        if (current_sequence_length > stats.max_sequence_length) {
            stats.max_sequence_length = current_sequence_length;
        }
        if (current_sequence_length < stats.min_sequence_length) {
            stats.min_sequence_length = current_sequence_length;
        }
        stats.total_sequence_length += current_sequence_length;
    }

    fclose(fp);
    return stats;
}

// دالة عشان نطبع الإحصائيات
void print_statistics(const GenomeStats *stats, const char *filename) {
    printf("\n=== Genome Statistics for %s ===\n", filename);
    printf("Number of sequences: %d\n", stats->sequence_count);
    printf("Total bases: %lld\n", stats->total_bases);
    printf("Total sequence length: %lld\n", stats->total_sequence_length);

    if (stats->sequence_count > 0) {
        printf("Average sequence length: %.2f\n",
               (double)stats->total_sequence_length / stats->sequence_count);
    }

    if (stats->total_bases > 0) {
        printf("GC content: %.2f%%\n",
               (double)stats->gc_count * 100.0 / stats->total_bases);
        printf("AT content: %.2f%%\n",
               (double)stats->at_count * 100.0 / stats->total_bases);
        printf("N content: %.2f%%\n",
               (double)stats->n_count * 100.0 / stats->total_bases);
        printf("Other bases: %.2f%%\n",
               (double)stats->other_count * 100.0 / stats->total_bases);
    }

    if (stats->sequence_count > 0) {
        printf("Max sequence length: %lld\n", stats->max_sequence_length);
        printf("Min sequence length: %lld\n", stats->min_sequence_length);
    }

    printf("Base composition:\n");
    printf("  A/T: %lld\n", stats->at_count);
    printf("  G/C: %lld\n", stats->gc_count);
    printf("  N: %lld\n", stats->n_count);
    printf("  Other: %lld\n", stats->other_count);
}

// الدالة الرئيسية مع معالجة وسائط سطر الأوامر
int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <fasta_file>\n", argv[0]);
        return 1;
    }

    const char *filename = argv[1];
    clock_t start_time = clock();

    printf("Processing FASTA file: %s\n", filename);
    GenomeStats stats = process_fasta_file(filename);

    clock_t end_time = clock();
    double execution_time = (double)(end_time - start_time) / CLOCKS_PER_SEC;

    print_statistics(&stats, filename);
    printf("\nExecution time: %.2f seconds\n", execution_time);

    return 0;
}
