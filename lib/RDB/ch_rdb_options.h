/*
 *
 *      Filename: ch_rdb_options.h
 *
 *        Author: shajf,csp001314@gmail.com
 *   Description: ---
 *        Create: 2018-04-19 12:19:17
 * Last Modified: 2018-04-19 12:19:17
 */

#ifndef CH_RDB_OPTIONS_H
#define CH_RDB_OPTIONS_H

typedef struct ch_rdb_options_t ch_rdb_options_t;

struct ch_rdb_options_t {


	int total_threads; /*rocksdb_options_increase_parallelism*/
	uint64_t block_cache_size_mb; /*rocksdb_options_optimize_for_point_lookup*/
	uint64_t ls_memtable_memory_budget; /*rocksdb_options_optimize_level_style_compaction*/
	uint64_t us_memtable_memory_budget; /*rocksdb_options_optimize_universal_style_compaction*/
	size_t readahead_size; /*rocksdb_options_compaction_readahead_size*/
	size_t num_levels; /*rocksdb_options_set_compression_per_level*/

	unsigned char allow_ingest_behind; /*rocksdb_options_set_allow_ingest_behind*/
	unsigned char create_if_missing; /*rocksdb_options_set_create_if_missing*/
	unsigned char create_missing_column_families; /*rocksdb_options_set_create_if_missing*/
	unsigned char error_if_exists; /*rocksdb_options_set_error_if_exists*/
	unsigned char paranoid_checks; /*rocksdb_options_set_paranoid_checks*/

	size_t num_paths; /*rocksdb_options_set_db_paths*/
	int log_level; /*rocksdb_options_set_info_log_level*/
	size_t write_buffer_size; /*rocksdb_options_set_write_buffer_size*/
	size_t db_write_buffer_size; /*rocksdb_options_set_db_write_buffer_size*/
	int max_open_files; /*rocksdb_options_set_max_open_files*/
	int max_file_opening_threads; /*rocksdb_options_set_max_file_opening_threads*/
	uint64_t max_total_wal_size; /*rocksdb_options_set_max_total_wal_size*/

	/*set_compression_options*/
	int w_bits;
	int level;
	int strategy;            
	int max_dict_bytes;
	/*end set_compression_options*/

	int num_levels; /*rocksdb_options_set_num_levels*/
	int level0_file_num_compaction_trigger; /*rocksdb_options_set_level0_file_num_compaction_trigger*/
	int level0_slowdown_writes_trigger; /*rocksdb_options_set_level0_slowdown_writes_trigger*/
	int max_mem_compaction_level; /*rocksdb_options_set_max_mem_compaction_level*/
	uint64_t target_file_size_base; /*rocksdb_options_set_target_file_size_base*/
	int target_file_size_multiplier; /*rocksdb_options_set_target_file_size_multiplier*/
	uint64_t max_bytes_for_level_base; /*rocksdb_options_set_max_bytes_for_level_base*/

};

#endif /*CH_RDB_OPTIONS_H*/
