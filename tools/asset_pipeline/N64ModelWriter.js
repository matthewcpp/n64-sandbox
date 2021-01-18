const N64Defs = require("./N64Defs");

const path  = require("path");
const fs = require("fs")

function writeHeader(model, outputFolder) {
    const filePath = path.join(outputFolder, `${model.name}.h`)
    console.log(`write: ${filePath}`);

    const file = fs.openSync(filePath, "w");
    fs.writeSync(file, `#ifndef MODEL_${model.name.toUpperCase()}_H\n`);
    fs.writeSync(file, `#define MODEL_${model.name.toUpperCase()}_H\n\n`);

    fs.writeSync(file, `const float ${model.name}_bounding_box[] = { ${model.bounding.toArrayStr()} };\n\n`);

    for (let i = 0; i < model.meshes.length; i++) {
        //console.log(`Mesh: ${model.meshes[i].vertices.length} vertices, ${model.meshes[i].triangles.length} triangles`);
        const slices = model.meshes[i].slice();
        const vertexArrayVar = `${model.name}_mesh_${i}`;

        // Write out mesh vertices for all slices in this mesh
        fs.writeSync(file, `const Vtx ${vertexArrayVar}[] = {\n`);
        for (const slice of slices) {
            for (const v of slice.vertices) {
                fs.writeSync(file, `{ ${v[0]}, ${v[1]}, ${v[2]}, ${v[3]}, ${v[4]}, ${v[5]}, ${v[6]}, ${v[7]}, ${v[8]}, ${v[9]} },\n`)
            }
        }
        fs.writeSync(file, `};\n\n`);

        // write out the mesh display list
        const displayListVar = vertexArrayVar + "_dl"
        fs.writeSync(file, `const Gfx ${displayListVar}[] = {\n`);

        let vertexListOffset = 0;

        for (let i = 0; i < slices.length; i++) {
            const slice = slices[i];
            // set the vertex pointer
            fs.writeSync(file, `gsSPVertex(${vertexArrayVar} + ${vertexListOffset}, ${slice.vertices.length}, 0),\n`);

            // write out all the triangles
            for (const t of slice.triangles) {
                fs.writeSync(file, `gsSP1Triangle(${t[0]}, ${t[1]}, ${t[2]}, 0),\n`)
            }

            vertexListOffset += slice.vertices.length;
        }

        // end the display list
        fs.writeSync(file, "gsSPEndDisplayList()\n");
        fs.writeSync(file, `};\n\n`);
    }

    fs.writeSync(file, `#endif`);
    fs.closeSync(file);
}

module.exports = {
    writeHeader: writeHeader
};
