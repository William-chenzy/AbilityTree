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
    
    const clickedTab = Array.from(tabs).find(tab => tab.textContent.trim() === document.querySelector(`#${tabId} h2`).textContent.trim());
    if (clickedTab) {
        clickedTab.classList.add('active');
        clickedTab.setAttribute('data-now', 'true');
    }
}

document.addEventListener('DOMContentLoaded', () => {
    showTab('tab1');
});
