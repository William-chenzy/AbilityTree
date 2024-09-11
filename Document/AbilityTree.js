const version = [
    '1.0.1',
    '1.1.1',
    '1.1.0',
    '1.1.0',
    '1.0.0',
    '1.1.0',
    '1.1.0',
    '1.0.1',
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
    'InjectionTool',
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
    link.href = './AbilityTreeInstallApp.exe';
    link.download = 'AbilityTreeInstallApp.exe';
    document.body.appendChild(link);
    link.click();
    document.body.removeChild(link);
}

function downloadLinux(){
    const link = document.createElement('a');
    link.href = './AbilityTreeInstallApp.zip';
    link.download = 'AbilityTree.zip';
    document.body.appendChild(link);
    link.click();
    document.body.removeChild(link);
}

function GetDocument(){
    const link = document.createElement('a');
    link.href = './Document.zip';
    link.download = 'AbilityTree_Document.zip';
    document.body.appendChild(link);
    link.click();
    document.body.removeChild(link);
}

function clickedImage() {
    window.open(Key[currentIndex]);
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

function exampleAndTool() {
    window.open("ExampleAndTools");
}

function showTab(tabId) {
    const contents = document.querySelectorAll('.tab-content');
    contents.forEach(content => {
        content.classList.remove('active');
    });

    const tabs = document.querySelectorAll('.tab');
    tabs.forEach(tab => {
        tab.classList.remove('active');
        tab.setAttribute('data-now', 'false');
    });
    const currentTab = document.querySelector(`.tab[data-tab="${tabId}"]`);
    currentTab.setAttribute('data-now', 'true');

    document.getElementById(tabId).classList.add('active');
    
    const clickedTab = Array.from(tabs).find(tab => tab.textContent.trim() === document.querySelector(`#${tabId}`).textContent.trim());
    if (clickedTab) {
        clickedTab.classList.add('active');
        clickedTab.setAttribute('data-now', 'true');
    }
}

document.addEventListener('DOMContentLoaded', () => {
    showTab('tab1');
});

setInterval(function(){nextImage()}, 5000);

showImage(currentIndex);
