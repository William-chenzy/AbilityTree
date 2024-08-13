const version = [
    '1.0.0',
    '1.1.0',
    '1.1.0',
    '1.1.0',
    '1.0.0',
    '1.1.0',
    '1.1.0',
    '1.0.0',
    '1.0.0',
];
const Key = [
    'AbilityTreeViewer',
    'CommunicationTool',
    'NetConfigTool',
    'PeFileAnalyzer',
    'PhotosTool',
    'QuickMacro',
    'Wireshark',
    'InstallTool',
    'PackagedTool',
];

let currentIndex = 0;

const module_ver = document.getElementById('module_version');
const imgElement = document.getElementById('current-image');
const thumbnails = document.querySelectorAll('.thumbnail');

function showImage(index) {
    currentIndex = index;
    module_name.innerHTML = Key[currentIndex]
    imgElement.src = "img/" + Key[currentIndex] + ".png";
    module_ver.innerHTML = "模组版本：" + version[currentIndex];
    updateThumbnails();
}

function downloadWindows(){
    const link = document.createElement('a');
    link.href = './AbilityTreeInstallApp_1.0.0.exe'; // 文件路径
    link.download = 'AbilityTreeInstallApp.exe'; // 下载时的文件名
    document.body.appendChild(link);
    link.click();
    document.body.removeChild(link);
}

function downloadLinux(){
    const link = document.createElement('a');
    link.href = './AbilityTreeInstallApp_1.0.0.zip'; // 文件路径
    link.download = 'AbilityTree_1.0.0.zip'; // 下载时的文件名
    document.body.appendChild(link);
    link.click();
    document.body.removeChild(link);
}

function clickedImage() {
    window.open("img/" + Key[currentIndex] + ".png");
}

function prevImage() {
    currentIndex = (currentIndex - 1 + Key.length) % Key.length;
    showImage(currentIndex);
}

function nextImage() {
    currentIndex = (currentIndex + 1) % Key.length;
    showImage(currentIndex);
}

function updateThumbnails() {
    thumbnails.forEach((thumb, index) => {
        thumb.classList.toggle('active', index === currentIndex);
    });
}

function gitSource() {
    window.open("https://github.com/William-chenzy/AbilityTree.git");
}

setInterval(function(){nextImage()}, 5000);

showImage(currentIndex);

