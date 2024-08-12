// scripts.js
const images = [
    'C:\\Users\\William_Chenzy\\Desktop\\AbilityTree\\include\\res\\close.png',
    'C:\\Users\\William_Chenzy\\Desktop\\AbilityTree\\include\\res\\close_hover.png',
    'C:\\Users\\William_Chenzy\\Desktop\\AbilityTree\\include\\res\\close_pressed.png',
    // Add more images as needed
];
const version = [
    '1.0.0',
    '1.0.0',
    '1.0.0',
    // Add more images as needed
];
const path = [
    'C:\\Users\\William_Chenzy\\Desktop\\AbilityTree\\include\\res\\close.png',
    'C:\\Users\\William_Chenzy\\Desktop\\AbilityTree\\include\\res\\close_hover.png',
    'C:\\Users\\William_Chenzy\\Desktop\\AbilityTree\\include\\res\\close_pressed.png',
    // Add more images as needed
];

let currentIndex = 0;

const module_ver = document.getElementById('module_version');
const imgElement = document.getElementById('current-image');
const thumbnails = document.querySelectorAll('.thumbnail');

function showImage(index) {
    currentIndex = index;
    imgElement.src = images[currentIndex];
    module_ver.innerHTML = "模组版本：" + version[currentIndex];
    updateThumbnails();
}

function downloadWindows(){
    const link = document.createElement('a');
    link.href = 'C:/Users/William_Chenzy/Desktop/AbilityTree/bin/win_x64/AbilityTreeInstallApp.exe'; // 文件路径
    link.download = 'AbilityTreeInstallApp.exe'; // 下载时的文件名
    document.body.appendChild(link);
    link.click();
    document.body.removeChild(link);
}

function downloadLinux(){
    const link = document.createElement('a');
    link.href = 'C:/Users/William_Chenzy/Desktop/AbilityTree/bin/win_x64/AbilityTreeInstallApp.exe'; // 文件路径
    link.download = 'AbilityTreeInstallApp'; // 下载时的文件名
    document.body.appendChild(link);
    link.click();
    document.body.removeChild(link);
}

function clickedImage() {
    window.open(path[currentIndex]);
}

function prevImage() {
    currentIndex = (currentIndex - 1 + images.length) % images.length;
    showImage(currentIndex);
}

function nextImage() {
    currentIndex = (currentIndex + 1) % images.length;
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

showImage(currentIndex);
