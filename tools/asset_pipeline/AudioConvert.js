const N64SoundBank = require("./N64SoundBank");
const N64AudioWriter = require("./N64AudioWriter");

const fs = require("fs");
const path = require("path");

function writeSoundBankFile(soundBankPath, ctlFilePath, tblFilePath, fileCount) {
    const ctrlFileStats = fs.statSync(ctlFilePath);

    const soundBankBuffer = Buffer.alloc(8);
    soundBankBuffer.writeUInt32BE(fileCount, 0);
    soundBankBuffer.writeUInt32BE(ctrlFileStats.size, 4);

    const soundBankFile = fs.openSync(soundBankPath, "w");
    fs.writeSync(soundBankFile, soundBankBuffer);
    fs.writeSync(soundBankFile, fs.readFileSync(ctlFilePath));
    fs.writeSync(soundBankFile, fs.readFileSync(tblFilePath));
    fs.closeSync(soundBankFile);
}

async function convertSoundBank(files, name, outputDir, archive) {
    const soundBank = new N64SoundBank(name);
    soundBank.load(files);
    await N64AudioWriter.writeSoundBank(soundBank);

    const soundBankPath = path.join(outputDir, `${soundBank.name}.soundbank`);
    writeSoundBankFile(soundBankPath, soundBank.ctlFilePath, soundBank.tblFilePath, soundBank.fileCount);
    archive.add(soundBankPath, "soundbank");

    soundBank.cleanup();
}

async function convertMusicBank(files, name, outputDir, archive) {
    const tempCtlFilePath = "/home/matthew/development/scratch/nusys/midi.ctl";
    const tempTblFilePath = "/home/matthew/development/scratch/nusys/midi.tbl";
    const tempSeqBankPath = "/home/matthew/development/scratch/nusys/midi.sbk";

    const instrumentBankPath = path.join(outputDir, `${name}.instrumentbank`);
    writeSoundBankFile(instrumentBankPath, tempCtlFilePath, tempTblFilePath, 0);
    const entry = archive.add(instrumentBankPath, "instrumentbank");

    const sbkFileStats = fs.statSync(tempSeqBankPath);

    const musicBankHeader = Buffer.alloc(12);
    musicBankHeader.writeUInt32BE(3, 0);
    musicBankHeader.writeUInt32BE(entry.index, 4);
    musicBankHeader.writeUInt32BE(sbkFileStats.size, 8);

    const musicBankPath = path.join(outputDir, `${name}.musicbank`);
    const musicBankFile = fs.openSync(musicBankPath, "w");
    fs.writeSync(musicBankFile, musicBankHeader);
    fs.writeSync(musicBankFile, fs.readFileSync(tempSeqBankPath));
    fs.closeSync(musicBankFile);

    archive.add(musicBankPath, "musicbank");
}

module.exports = {
    convertSoundBank: convertSoundBank,
    convertMusicBank: convertMusicBank
};
