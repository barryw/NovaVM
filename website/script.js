(() => {
    const buttons = document.querySelectorAll("[data-copy-target]");

    buttons.forEach((button) => {
        button.addEventListener("click", async () => {
            const targetId = button.getAttribute("data-copy-target");
            const target = targetId ? document.getElementById(targetId) : null;

            if (!target) {
                return;
            }

            const text = target.textContent.trim();

            try {
                await navigator.clipboard.writeText(text);
                button.textContent = "Copied";
                window.setTimeout(() => {
                    button.textContent = "Copy";
                }, 1800);
            } catch {
                const range = document.createRange();
                range.selectNodeContents(target);
                const selection = window.getSelection();
                selection.removeAllRanges();
                selection.addRange(range);
                button.textContent = "Selected";
            }
        });
    });
})();
