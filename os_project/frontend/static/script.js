async function createFile() {
    const filename = document.getElementById("filename").value;
    await fetch("/create_file", {
        method: "POST",
        headers: { "Content-Type": "application/json" },
        body: JSON.stringify({ filename })
    });
    listFiles();
}

async function createDir() {
    const filename = document.getElementById("filename").value;
    await fetch("/create_dir", {
        method: "POST",
        headers: { "Content-Type": "application/json" },
        body: JSON.stringify({ filename })
    });
    listFiles();
}

async function deleteEntry() {
    const filename = document.getElementById("filename").value;
    await fetch("/delete", {
        method: "POST",
        headers: { "Content-Type": "application/json" },
        body: JSON.stringify({ filename })
    });
    listFiles();
}

async function listFiles() {
    const res = await fetch("/list");
    const files = await res.json();
    document.getElementById("fileList").innerText = files.join("\n");
}

async function readFile() {
    const filename = document.getElementById("readFile").value;
    const res = await fetch("/read", {
        method: "POST",
        headers: { "Content-Type": "application/json" },
        body: JSON.stringify({ filename })
    });
    const data = await res.json();
    document.getElementById("fileContent").innerText = data.content;
}
