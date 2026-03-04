use byteorder::{ByteOrder, LittleEndian};
use clap::Parser;
use flate2::{Decompress, FlushDecompress};
use itertools::Itertools;
use regex::Regex;
use std::fs;
use std::io::BufRead;
use std::os::unix;
use std::path::{Path, PathBuf};
use std::process::Command;
use std::process::ExitCode;
use std::{env, io::Write};
use tempfile::tempdir;

/// Clap value parser function that verifies that a path (directory) exists
/// and converts it to its absolute path form.
fn parser_path_exists(s: &str) -> Result<PathBuf, String> {
    let path = fs::canonicalize(PathBuf::from(s))
        .map_err(|err| format!("Invalid path {s}: {err}"))?;

    if fs::exists(&path).is_ok_and(|b| b == true) && path.is_dir() {
        Ok(path)
    } else {
        Err(format!("Path {} does not exist or is not accessible", path.display()))
    }
}

/// Clap value parser function that verifies that a file exists
fn parser_file_exists(s: &str) -> Result<PathBuf, String> {
    let file = fs::canonicalize(PathBuf::from(s))
        .map_err(|err| format!("Invalid file {s}: {err}"))?;

    if fs::exists(&file).is_ok_and(|b| b == true) && file.is_file() {
        Ok(file)
    } else {
        Err(format!("File {} does not exist or is not accessible", file.display()))
    }
}

/// Clap value parser function that verifies that a given string is a valid version number
fn parser_valid_revision(s: &str) -> Result<String, String> {
    // Match any version number, e.g. X.Y, X.Y.Z, X.Y.Z.W, etc
    let re = Regex::new(r"^[0-9]+(\.[0-9]+){1,}$").unwrap();

    if re.is_match(s) {
        Ok(s.to_string())
    } else {
        Err(format!("Invalid revision \"{s}\""))
    }
}

#[derive(Parser, Debug)]
/// Extract firmware binaries from the OpenRM git repository in a format expected by the Nouveau device driver.
struct Cli {
    /// Path to source directory (where version.mk exists)
    #[arg (short, long,
        default_value=env::current_dir().unwrap().into_os_string(),
        value_parser=parser_path_exists)]
    input: PathBuf,

    /// Path to target directory (where files will be written)
    /// Unlike the Python version, the output directory must already exist.
    #[arg (short, long,
        default_value=env::current_dir().unwrap().into_os_string(),
        value_parser=parser_path_exists)]
    output: PathBuf,

    /// Files will be named with this version number
    #[arg (short, long,
        value_parser=parser_valid_revision)]
    revision: Option<String>,

    /// Path to Nvidia driver .run package, for also extracting the GSP-RM firmware
    #[arg (short, long,
        value_parser=parser_file_exists)]
    driver: Option<PathBuf>,

    /// Also create symlinks for all supported GPUs
    #[arg(short, long, action)]
    symlink: bool,

    /// Also generate a WHENCE file
    #[arg(short, long, action)]
    whence: bool,
}

/// Extract the driver/firmware version number from the version.mk file
fn find_version() -> Option<String> {
    let re = Regex::new(r"^NVIDIA_VERSION = ([^\s]+)").unwrap();
    let file = fs::File::open("version.mk").unwrap();
    let reader = std::io::BufReader::new(file);

    // Alternative approach: https://play.rust-lang.org/?version=stable&mode=debug&edition=2021&gist=fa91a4ad9024a836a7fd74de387755c1
    // Or https://play.rust-lang.org/?version=stable&mode=debug&edition=2021&gist=d3dd6b77c08a9bf9d16510dea9ba1a86

    for line in reader.lines() {
        let line = line.unwrap();
        if let Some(version) = re.captures(&line) {
            return Some(version[1].to_string());
        }
    }
    None
}

// Executes a shell command, and either returns stdout as success,
// or an error message as failure.
fn command(filename: &Path, parameter: &str) -> Result<String, String> {
    let output = Command::new("/bin/sh")
        .arg(filename)
        .arg(parameter)
        .output()
        .map_err(|err| format!("Could not execute {} {parameter}: {err}", filename.display()))?;

    if !output.status.success() {
        return Err(format!(
            "Command {} {parameter} failed with exit code {}.\n{}\n{}",
            filename.display(),
            output.status.code().unwrap_or(0),
            String::from_utf8_lossy(&output.stdout),
            String::from_utf8_lossy(&output.stderr)
        ));
    }

    Ok(String::from_utf8_lossy(&output.stdout).trim().to_string())
}

// Extract the GSP-RM firmware from the .run file and copy the binaries
// to the target directory.
fn gsp_firmware(output: &Path, version: &str, runfile: &Path) -> Result<(), String> {
    // When tempdir goes out of scope, its destructor will delete the directory
    let tempdir = tempdir()
        .map_err(|err| format!("Could not create temp directory: {err}"))?;

    // Chdir to the new temporary directory, because the .run file only extracts
    // to the current directory.
    env::set_current_dir(&tempdir)
        .map_err(|err| format!("Could not chdir to {}: {err}", tempdir.path().display()))?;

    let directory = command(runfile, "--target-directory")?;

    // Extract the firmware images from the .run file
    command(runfile, "-x")?;

    // Chdir to the directory that has the firmware images
    let imagedir = format!("{directory}/firmware");
    env::set_current_dir(&imagedir)
        .map_err(|err| format!("Could not chdir to {imagedir}: {err}"))?;

    let target = output.join(format!("tu102/gsp/gsp-{version}.bin"));
    let source = "gsp_tu10x.bin";
    fs::copy(source, &target)
        .map_err(|err| format!("Could not copy {source} to {}: {err}", target.display()))?;

    let target = output.join(format!("ga102/gsp/gsp-{version}.bin"));
    let source = "gsp_ga10x.bin";
    fs::copy(source, &target)
        .map_err(|err| format!("Could not copy {source} to {}: {err}", target.display()))?;

    Ok(())
}

fn whence(output: &Path, version: &str) -> Result<(), String> {
    let text = format!(
        "File: nvidia/tu102/gsp/bootloader-{version}.bin
File: nvidia/tu102/gsp/booter_load-{version}.bin
File: nvidia/tu102/gsp/booter_unload-{version}.bin
Link: nvidia/tu104/gsp -> ../tu102/gsp
Link: nvidia/tu106/gsp -> ../tu102/gsp

File: nvidia/tu116/gsp/booter_load-{version}.bin
File: nvidia/tu116/gsp/booter_unload-{version}.bin
Link: nvidia/tu116/gsp/bootloader-{version}.bin -> ../../tu102/gsp/bootloader-{version}.bin
Link: nvidia/tu117/gsp -> ../tu116/gsp

File: nvidia/ga100/gsp/bootloader-{version}.bin
File: nvidia/ga100/gsp/booter_load-{version}.bin
File: nvidia/ga100/gsp/booter_unload-{version}.bin

File: nvidia/ad102/gsp/bootloader-{version}.bin
File: nvidia/ad102/gsp/booter_load-{version}.bin
File: nvidia/ad102/gsp/booter_unload-{version}.bin
Link: nvidia/ad103/gsp -> ../ad102/gsp
Link: nvidia/ad104/gsp -> ../ad102/gsp
Link: nvidia/ad106/gsp -> ../ad102/gsp
Link: nvidia/ad107/gsp -> ../ad102/gsp

File: nvidia/ga102/gsp/bootloader-{version}.bin
File: nvidia/ga102/gsp/booter_load-{version}.bin
File: nvidia/ga102/gsp/booter_unload-{version}.bin
Link: nvidia/ga103/gsp -> ../ga102/gsp
Link: nvidia/ga104/gsp -> ../ga102/gsp
Link: nvidia/ga106/gsp -> ../ga102/gsp
Link: nvidia/ga107/gsp -> ../ga102/gsp

File: nvidia/tu102/gsp/gsp-{version}.bin
Origin: gsp_tu10x.bin from NVIDIA-Linux-x86_64-{version}.run
Link: nvidia/tu116/gsp/gsp-{version}.bin -> ../../tu102/gsp/gsp-{version}.bin
Link: nvidia/ga100/gsp/gsp-{version}.bin -> ../../tu102/gsp/gsp-{version}.bin

File: nvidia/ga102/gsp/gsp-{version}.bin
Origin: gsp_ga10x.bin from NVIDIA-Linux-x86_64-{version}.run
Link: nvidia/ad102/gsp/gsp-{version}.bin -> ../../ga102/gsp/gsp-{version}.bin
");

    let whence = "WHENCE.txt";
    let filename = output.join(whence);

    let mut file = fs::OpenOptions::new()
        .write(true)
        .create(true)
        .open(&filename)
        .map_err(|err| format!("Could not create {whence}: {err}"))?;

    file.write_all(text.as_bytes())
        .map_err(|err| format!("Could not write to {whence}: {err}"))?;

    file.sync_all()
        .map_err(|err| format!("Failed to sync {whence}: {err}"))?;

    Ok(())
}

/// Create a symlink if it doesn't exist already, or return an error
fn symlink(original: &Path, link: &Path) -> Result<(), String> {
    match fs::read_link(&link) {
        Ok(o) if o == original => {
            // Symlink already exists and is correct
        }
        Ok(o) => {
            // Symlink exists but points to wrong file for some reason
            return Err(format!("Symlink {} points to wrong item {}", link.display(), o.display()));
        }
        Err(err) if err.kind() == std::io::ErrorKind::NotFound => {
            // Symlink doesn't exist, so we can create it.
            unix::fs::symlink(&original, &link)
                .map_err(|err| format!("Could not create symlink {} -> {}: {err}", link.display(), original.display()))?;
        }
        Err(err) => {
            // Something else is wrong
            return Err(format!("File/symlink {} is broken: {err}", link.display()));
        }
    }

    Ok(())
}

/// Create symlinks in the target directory for the other GPUs.  This mirrors
/// what the WHENCE file in linux-firmware does.
fn symlinks(output: &Path, version: &str) -> Result<(), String> {
    env::set_current_dir(&output)
        .map_err(|err| format!("Could not chdir to {}: {err}", output.display()))?;

    // For these paths, we can just create a symlink of the entire 'gsp' directory.
    // For example, tu104, tu106, and tu117 all use the same binaries as tu102.
    let paths = [
        ("tu104", "tu102"),
        ("tu106", "tu102"),
        ("tu117", "tu102"),
        ("ad103", "ad102"),
        ("ad104", "ad102"),
        ("ad106", "ad102"),
        ("ad107", "ad102"),
        ("ga103", "ga102"),
        ("ga104", "ga102"),
        ("ga106", "ga102"),
        ("ga107", "ga102"),
    ];

    // Create the source directory for each GPU, and then create symlink
    // for the gsp directory to the "parent" GPU.
    for (source, dest) in paths {
        fs::create_dir_all(&source)
            .map_err(|err| format!("Could not create {}/{source}: {err}", output.display()))?;

        let original = PathBuf::from(format!("../{dest}/gsp"));
        let link = PathBuf::from(format!("{source}/gsp"));

        symlink(&original, &link)?;
    }

    // Create additional symlinks for specific firmware files.  TU116 is a special case
    // because it uses the same bootloader as TU102, but the booter images are different.
    let paths = [
        ("tu116", "tu102", "bootloader"),
        ("tu116", "tu102", "gsp"),
        ("ga100", "tu102", "gsp"),
        ("ad102", "ga102", "gsp"),
    ];

    for (source, dest, kind) in paths {
        // These directories should already exist, but create them anyway for consistency.
        let path = format!("{source}/gsp");
        fs::create_dir_all(&path)
            .map_err(|err| format!("Could not create {}/{path}: {err}", output.display()))?;

        let link = PathBuf::from(format!("{path}/{kind}-{version}.bin"));
        let original = PathBuf::from(format!("../../{dest}/gsp/{kind}-{version}.bin"));

        symlink(&original, &link)?;
    }

    Ok(())
}

fn get_bytes(filename: &Path, array: &str, expected_size: Option<usize>) -> Result<Vec<u8>, String> {
    let re_datasize = Regex::new(r"DATA SIZE \(bytes\): (\d+)").unwrap();
    let re_compressedsize = Regex::new(r"COMPRESSED SIZE \(bytes\): (\d+)").unwrap();
    let re_bytes = Regex::new(r"0x([0-9a-f][0-9a-f])[^0-9a-f]").unwrap();

    let file = fs::File::open(filename)
        .map_err(|err| format!("Could not open {}: {err}", filename.display()))?;
    let reader = std::io::BufReader::new(file);

    let mut compressed: bool = false;
    let mut data_size: usize = 0;
    let mut compressed_size: usize = 0;
    let mut bytes: Vec<u8> = vec![];
    let mut in_bytes: bool = false;

    for line in reader.lines() {
        let line = line.unwrap();
        if in_bytes {
            if line.contains("};") {
                break;
            }

            // Extract all of the two-digit hex numbers from the line and convert
            // them to a vector of u8.  captures_iter() will return an iterator only
            // over matches, ignoring everything else, even if there are 0 matches.
            let row = re_bytes
                .captures_iter(&line)
                .map(|c| u8::from_str_radix(&c[1], 16).unwrap());
            bytes.extend(row);
            continue;
        }
        if line.contains("COMPRESSION: NO") {
            compressed = false;
            continue;
        }
        if line.contains("COMPRESSION: YES") {
            compressed = true;
            continue;
        }
        if let Some(size) = re_datasize.captures(&line) {
            data_size = size[1].parse().unwrap();
            continue;
        }
        if let Some(size) = re_compressedsize.captures(&line) {
            compressed_size = size[1].parse().unwrap();
            continue;
        }
        if line.contains(&format!("static BINDATA_CONST NvU8 {array}")) {
            in_bytes = true;
            bytes.reserve_exact(if compressed {
                compressed_size
            } else {
                data_size
            });
            continue;
        }
    }

    if bytes.is_empty() {
        return Err(format!("Error: no data found for {array}"));
    }

    let output = if compressed {
        if bytes.len() != compressed_size {
            return Err(format!("compressed array {array} in {} should be {compressed_size} bytes but is actually {}",
                       filename.display(), bytes.len()));
        }
        let mut uncompressed = Vec::<u8>::with_capacity(data_size);
        let mut decompressor = Decompress::new(false);
        if let Err(err) =
            decompressor.decompress_vec(&bytes, &mut uncompressed, FlushDecompress::Finish)
        {
            return Err(format!("array {array} in {} decompressed to {} bytes but should have been {data_size} bytes: {err}",
                filename.display(), bytes.len()));
        }

        uncompressed
    } else {
        if bytes.len() != data_size {
            return Err(format!("array {array} in {} should be {data_size} bytes but is actually {}",
                filename.display(), bytes.len()));
        }

        bytes
    };

    if let Some(size) = expected_size {
        if size != output.len() {
            return Err(format!("array {array} in {} is {} bytes but should be {size} bytes",
                filename.display(), output.len()));
        }
    }

    Ok(output)
}

fn round_up_to_base(x: u32, base: u32) -> u32 {
    ((x + base - 1) / base) * base
}

fn bootloader(output: &Path, version: &str, gpu: &str, prod: &str) -> Result<(), String> {
    let gpu_upper = gpu.to_uppercase();
    let filename = PathBuf::from(format!(
        "src/nvidia/generated/g_bindata_kgspGetBinArchiveGspRmBoot_{gpu_upper}.c"
    ));

    println!("Creating nvidia/{gpu}/gsp/bootloader-{version}");

    let gsp = output.join(format!("{gpu}/gsp"));
    fs::create_dir_all(&gsp)
        .map_err(|err| format!("Could not create nvidia/{gpu}/gsp/: {err}"))?;

    // Extract the actual bootloader firmware
    let array = format!("kgspBinArchiveGspRmBoot_{gpu_upper}_ucode_image_{prod}data");
    let firmware = get_bytes(&filename, &array, None)?;
    let firmware_size: u32 = firmware.len() as u32;

    // Extract the descriptor (RM_RISCV_UCODE_DESC)
    let array = format!("kgspBinArchiveGspRmBoot_{gpu_upper}_ucode_desc_{prod}data");
    let descriptor = get_bytes(&filename, &array, None)?;
    let descriptor_size: u32 = descriptor.len() as u32;

    // Create the output
    let binfile: PathBuf = gsp.join(format!("bootloader-{version}.bin"));
    let mut file = fs::OpenOptions::new().write(true).create(true).open(&binfile)
        .map_err(|err| format!("Could not create {}: {err}", binfile.display()))?;

    // First, add the nvfw_bin_hdr header
    let total_size: u32 = round_up_to_base(24 + firmware_size + descriptor_size, 256);
    let firmware_offset: u32 = 24 + descriptor_size;

    let nvfw_bin_hdr = [0x10de, 1, total_size, 24, firmware_offset, firmware_size]
        .iter()
        .flat_map(|x| x.to_le_bytes())
        .collect::<Vec<_>>();
    file.write_all(&nvfw_bin_hdr)
        .map_err(|err| format!("Could not write to {}: {err}", filename.display()))?;

    // Second, add the descriptor
    file.write_all(&descriptor)
        .map_err(|err| format!("Could not write to {}: {err}", filename.display()))?;

    // Finally, the actual bootloader image
    file.write_all(&firmware)
        .map_err(|err| format!("Could not write to {}: {err}", filename.display()))?;

    file.sync_all()
        .map_err(|err| format!("Failed to sync {}: {err}", filename.display()))?;

    Ok(())
}

trait StrExt {
    fn capitalize_first_letter(&self) -> String;
    fn replace_last_with_x(&self) -> String;
}

impl StrExt for str {
    fn capitalize_first_letter(&self) -> String {
        let (first, rest) = self.split_at(1);

        first.to_ascii_uppercase() + rest
    }

    fn replace_last_with_x(&self) -> String {
        format!("{}X", &self[..self.len() - 1])
    }
}

/// GSP Booter load and unload
fn booter(output: &Path, version: &str, gpu: &str, load: &str, sigsize: u32) -> Result<(), String> {
    let gpu_upper = gpu.to_uppercase();
    let load_upper: String = load.capitalize_first_letter();
    let filename: PathBuf = PathBuf::from(format!(
        "src/nvidia/generated/g_bindata_kgspGetBinArchiveBooter{load_upper}Ucode_{gpu_upper}.c"
    ));

    println!("Creating nvidia/{gpu}/gsp/booter_{load}-{version}.bin");

    let gsp: PathBuf = output.join(format!("{gpu}/gsp"));
    fs::create_dir_all(&gsp)
        .map_err(|err| format!("Could not create nvidia/{gpu}/gsp/: {err}"))?;

    // Extract the actual booter firmware
    let array = format!("kgspBinArchiveBooter{load_upper}Ucode_{gpu_upper}_image_prod_data");
    let firmware = get_bytes(&filename, &array, None)?;
    let firmware_size = firmware.len() as u32;

    // Extract the signatures
    let array = format!("kgspBinArchiveBooter{load_upper}Ucode_{gpu_upper}_sig_prod_data");
    let signatures = get_bytes(&filename, &array, None)?;
    let signatures_size = signatures.len() as u32;
    if (signatures_size % sigsize) != 0 {
        return Err(format!("signature file size for {array} is uneven value of {sigsize}"));
    }
    let num_sigs = signatures_size / sigsize;
    if num_sigs < 1 {
        return Err(format!("invalid number of signatures: {num_sigs}"));
    }

    // Extract the patch location
    let array = format!("kgspBinArchiveBooter{load_upper}Ucode_{gpu_upper}_patch_loc_data");
    let patch_loc_data = get_bytes(&filename, &array, Some(4))?;
    let patch_loc = LittleEndian::read_u32(&patch_loc_data);

    // Extract the patch meta variables
    let array = format!("kgspBinArchiveBooter{load_upper}Ucode_{gpu_upper}_patch_meta_data");
    let patch_meta_data = get_bytes(&filename, &array, Some(12))?;

    let (fuse_ver, engine_id, ucode_id) = patch_meta_data
        .chunks(4)
        .map(LittleEndian::read_u32)
        .collect_tuple()
        .unwrap();

    let binfile: PathBuf = gsp.join(format!("booter_{load}-{version}.bin"));
    let mut file = fs::OpenOptions::new().write(true).create(true).open(&binfile)
        .map_err(|err| format!("Could not create {}: {err}", binfile.display()))?;

    // First, add the nvfw_bin_hdr header
    let total_size: u32 = round_up_to_base(120 + signatures_size + firmware_size, 256);
    let firmware_offset: u32 = 120 + signatures_size;

    let nvfw_bin_hdr = [0x10de, 1, total_size, 24, firmware_offset, firmware_size]
        .iter()
        .flat_map(|x| x.to_le_bytes())
        .collect::<Vec<u8>>();
    file.write_all(&nvfw_bin_hdr)
        .map_err(|err| format!("Could not write to {}: {err}", filename.display()))?;

    // Second, add the nvfw_hs_header_v2 header
    let patch_loc_offset = 60 + signatures_size;
    let patch_sig_offset = patch_loc_offset + 4;
    let meta_data_offset = patch_sig_offset + 4;
    let num_sig_offset = meta_data_offset + 12;
    let header_offset = num_sig_offset + 4;

    let nvfw_hs_header_v2 = [
        60,
        signatures_size,
        patch_loc_offset,
        patch_sig_offset,
        meta_data_offset,
        12,
        num_sig_offset,
        header_offset,
        36,
    ]
        .iter()
        .flat_map(|x| x.to_le_bytes())
        .collect::<Vec<u8>>();
    file.write_all(&nvfw_hs_header_v2)
        .map_err(|err| format!("Could not write to {}: {err}", filename.display()))?;

    // Third, the actual signatures
    file.write_all(&signatures)
        .map_err(|err| format!("Could not write to {}: {err}", filename.display()))?;

    //  Fourth, patch_loc[], patch_sig[], fuse_ver, engine_id, ucode_id, and num_sigs
    let patch_meta = [patch_loc, 0, fuse_ver, engine_id, ucode_id, num_sigs]
        .iter()
        .flat_map(|x| x.to_le_bytes())
        .collect::<Vec<u8>>();
    file.write_all(&patch_meta)
        .map_err(|err| format!("Could not write to {}: {err}", filename.display()))?;

    // Extract the descriptor (nvkm_gsp_booter_fw_hdr)
    let array = format!("kgspBinArchiveBooter{load_upper}Ucode_{gpu_upper}_header_prod_data");
    let descriptor = get_bytes(&filename, &array, Some(36))?;

    // Fifth, the descriptor
    file.write_all(&descriptor)
        .map_err(|err| format!("Could not write to {}: {err}", filename.display()))?;

    // And finally, the actual scrubber image
    file.write_all(&firmware)
        .map_err(|err| format!("Could not write to {}: {err}", filename.display()))?;

    file.sync_all()
        .map_err(|err| format!("Failed to sync {}: {err}", filename.display()))?;

    Ok(())
}

/// GPU memory scrubber, needed for some GPUs and configurations
#[allow(dead_code)]
fn scrubber(output: &Path, version: &str, gpu: &str, sigsize: u32) -> Result<(), String> {
    // Unfortunately, RM breaks convention with the scrubber image and labels
    // the files and arrays with AD10X instead of AD102.
    let gpux = gpu.replace_last_with_x().to_uppercase();

    let filename = PathBuf::from(
        format!("src/nvidia/generated/g_bindata_ksec2GetBinArchiveSecurescrubUcode_{gpux}.c"));

    println!("Creating nvidia/{gpu}/gsp/scrubber-{version}.bin");

    let gsp: PathBuf = output.join(format!("{gpu}/gsp"));
    fs::create_dir_all(&gsp)
        .map_err(|err| format!("Could not create nvidia/{gpu}/gsp/: {err}"))?;

    // Extract the actual scrubber firmware
    let array = format!("ksec2BinArchiveSecurescrubUcode_{gpux}_image_prod_data");
    let firmware = get_bytes(&filename, &array, None)?;
    let firmware_size = firmware.len() as u32;

    // Extract the signatures
    let array = format!("ksec2BinArchiveSecurescrubUcode_{gpux}_sig_prod_data");
    let signatures = get_bytes(&filename, &array, None)?;
    let signatures_size = signatures.len() as u32;
    if (signatures_size % sigsize) != 0 {
        return Err(format!("signature file size for {array} is uneven value of {sigsize}"));
    }
    let num_sigs = signatures_size / sigsize;
    if num_sigs < 1 {
        return Err(format!("invalid number of signatures: {num_sigs}"));
    }

    // Extract the patch location
    let array = format!("ksec2BinArchiveSecurescrubUcode_{gpux}_patch_loc_data");
    let patch_loc_data = get_bytes(&filename, &array, Some(4))?;
    let patch_loc = LittleEndian::read_u32(&patch_loc_data);

    // Extract the patch meta variables
    let array = format!("ksec2BinArchiveSecurescrubUcode_{gpux}_patch_meta_data");
    let patch_meta_data = get_bytes(&filename, &array, Some(12))?;

    let (fuse_ver, engine_id, ucode_id) = patch_meta_data
        .chunks(4)
        .map(LittleEndian::read_u32)
        .collect_tuple()
        .unwrap();

    let binfile = gsp.join(format!("scrubber-{version}.bin"));
    let mut file = fs::OpenOptions::new().write(true).create(true).open(&binfile)
        .map_err(|err| format!("Could not create {}: {err}", binfile.display()))?;

    // First, add the nvfw_bin_hdr header
    let total_size: u32 = round_up_to_base(120 + signatures_size + firmware_size, 256);
    let firmware_offset: u32 = 120 + signatures_size;

    let nvfw_bin_hdr = [0x10de, 1, total_size, 24, firmware_offset, firmware_size]
        .iter()
        .flat_map(|x| x.to_le_bytes())
        .collect::<Vec<u8>>();
    file.write_all(&nvfw_bin_hdr)
        .map_err(|err| format!("Could not write to {}: {err}", filename.display()))?;

    // Second, add the nvfw_hs_header_v2 header
    let patch_loc_offset = 60 + signatures_size;
    let patch_sig_offset = patch_loc_offset + 4;
    let meta_data_offset = patch_sig_offset + 4;
    let num_sig_offset = meta_data_offset + 12;
    let header_offset = num_sig_offset + 4;

    let nvfw_hs_header_v2 = [
        60,
        signatures_size,
        patch_loc_offset,
        patch_sig_offset,
        meta_data_offset,
        12,
        num_sig_offset,
        header_offset,
        36,
    ]
        .iter()
        .flat_map(|x| x.to_le_bytes())
        .collect::<Vec<u8>>();
    file.write_all(&nvfw_hs_header_v2)
        .map_err(|err| format!("Could not write to {}: {err}", filename.display()))?;

    // Third, the actual signatures
    file.write_all(&signatures)
        .map_err(|err| format!("Could not write to {}: {err}", filename.display()))?;

    //  Fourth, patch_loc[], patch_sig[], fuse_ver, engine_id, ucode_id, and num_sigs
    let patch_meta = [patch_loc, 0, fuse_ver, engine_id, ucode_id, num_sigs]
        .iter()
        .flat_map(|x| x.to_le_bytes())
        .collect::<Vec<u8>>();
    file.write_all(&patch_meta)
        .map_err(|err| format!("Could not write to {}: {err}", filename.display()))?;

    // Extract the descriptor (nvkm_gsp_booter_fw_hdr)
    let array = format!("ksec2BinArchiveSecurescrubUcode_{gpux}_header_prod_data");
    let descriptor = get_bytes(&filename, &array, Some(36))?;

    // Fifth, the descriptor
    file.write_all(&descriptor)
        .map_err(|err| format!("Could not write to {}: {err}", filename.display()))?;

    // And finally, the actual scrubber image
    file.write_all(&firmware)
        .map_err(|err| format!("Could not write to {}: {err}", filename.display()))?;

    file.sync_all()
        .map_err(|err| format!("Failed to sync {}: {err}", filename.display()))?;

    Ok(())
}

fn main() -> ExitCode {
    let args = Cli::parse();

    // If -i was specified, then we start by chdir to that path
    if let Err(err) = env::set_current_dir(&args.input) {
        println!("Error: could not chdir to {}: {err}", args.input.display());
        return ExitCode::from(2);
    }

    // version.mk must exist in the current directory
    match fs::exists("version.mk") {
        Ok(false) | Err(_) => {
            println!("Error: version.mk not found");
            return ExitCode::from(2);
        }
        _ => (),
    }

    // If -r is passed, then use that as the version number.
    // Otherwise, extract it from version.mk
    let version: String = if let Some(r) = &args.revision {
        r.clone()
    } else {
        let v = find_version();
        if v == None {
            println!("Error: could not determine firmware version");
            return ExitCode::from(1);
        }
        v.unwrap()
    };

    println!("Generating files for version {version}");

    // Create the "nvidia" subdir where all the files will go
    let output = args.output.join("nvidia");
    match fs::create_dir(&output) {
        Ok(_) => println!("Writing files to {}.", output.display()),
        Err(e) if e.kind() == std::io::ErrorKind::AlreadyExists =>
            println!("Overwriting files in {}", output.display()),
        Err(e) => {
            println!("Error: could not create \"nvidia\" target directory: {e}");
            return ExitCode::from(1);
        }
    }

    let booters = [
        ("tu102", "load", 16),
        ("tu102", "unload", 16),
        ("tu116", "load", 16),
        ("tu116", "unload", 16),
        ("ga100", "load", 384),
        ("ga100", "unload", 384),
        ("ga102", "load", 384),
        ("ga102", "unload", 384),
        ("ad102", "load", 384),
        ("ad102", "unload", 384),
    ];
    for (gpu, load, sigsize) in booters {
        if let Err(err) = booter(&output, &version, gpu, load, sigsize) {
            println!("{err}");
            return ExitCode::from(1);
        }
    }

    let bootloaders = [
        ("tu102", ""),
        ("ga100", ""),
        ("ga102", "prod_"),
        ("ad102", "prod_"),
    ];
    for (gpu, prod) in bootloaders {
        if let Err(err) = bootloader(&output, &version, gpu, prod) {
            println!("{err}");
            return ExitCode::from(1);
        }
    }

    // Scrubber is currently not used, but let's generate the files anyway
    if let Err(err) = scrubber(&output, &version, "ad102", 384) {
        println!("{err}");
        return ExitCode::from(1);
    }

    if args.driver.is_some() {
        let driver = args.driver.unwrap();
        println!("Extracting GSP-RM firmware from {}", driver.display());
        if let Err(err) = gsp_firmware(&output, &version, &driver) {
            println!("{err}");
            return ExitCode::from(1);
        }
    }

    if args.whence {
        // Unlike all other output, the whence file goes in the root of the target
        // directory.
        println!("Creating {}/WHENCE.txt", args.output.display());
        if let Err(err) = whence(&args.output, &version) {
            println!("{err}");
            return ExitCode::from(1);
        }
    }

    if args.symlink {
        println!("Creating symlinks in {}", output.display());
        if let Err(err) = symlinks(&output, &version) {
            println!("{err}");
            return ExitCode::from(1);
        }
    }

    ExitCode::SUCCESS
}
