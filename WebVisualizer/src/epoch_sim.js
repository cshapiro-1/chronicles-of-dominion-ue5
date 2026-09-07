
const THEMES = {
    alluvial: { name: "Sun-Drenched Alluvial (Akkad)", bg: "#1c170d", sand: "#8c7247", river: "#0ea5e9", banks: "#22543d", wall: "#d97706", roof: "#b45309", trim: "#fbbf24", vfx: "rgba(251, 191, 36, 0.4)" },
    grimdark: { name: "Mythic Grimdark Smog (Ur-Ghul)", bg: "#09090b", sand: "#27272a", river: "#7f1d1d", banks: "#1c1917", wall: "#3f3f46", roof: "#18181b", trim: "#ef4444", vfx: "rgba(239, 68, 68, 0.5)" },
    classical: { name: "Classical Imperial Marble (Athenia)", bg: "#0f172a", sand: "#334155", river: "#0284c7", banks: "#166534", wall: "#f8fafc", roof: "#dc2626", trim: "#f59e0b", vfx: "rgba(56, 189, 248, 0.4)" },
    nordic: { name: "Nordic Timber & Mist (Norsgard)", bg: "#030712", sand: "#1f2937", river: "#06b6d4", banks: "#064e3b", wall: "#78350f", roof: "#15803d", trim: "#38bdf8", vfx: "rgba(203, 213, 225, 0.3)" }
};

const EPOCHS = [
    { name: "BRONZE AGE (3,000 BCE)", crest: "🥉", progress: 25 },
    { name: "CLASSICAL IRON (500 BCE)", crest: "🏛️", progress: 50 },
    { name: "FEUDAL HIGH MEDIEVAL (1100 CE)", crest: "🏰", progress: 75 },
    { name: "INDUSTRIAL STEAM (1850 CE)", crest: "🚂", progress: 100 }
];

class DominionRTSApp {
    constructor() {
        this.canvas = document.getElementById('rtsCanvas');
        this.ctx = this.canvas.getContext('2d');
        this.minimapCanvas = document.getElementById('minimapCanvas');
        this.miniCtx = this.minimapCanvas.getContext('2d');

        this.time = 0;
        this.currentEpoch = 0;
        this.currentTheme = 'alluvial';
        this.activePlacingType = null;

        this.resources = { food: 1250, wood: 840, gold: 5000, stone: 620, pop: 42, popCap: 60 };

        this.buildings = [
            { id: 1, x: 0.35, y: 0.38, type: 'ziggurat', name: 'Ziggurat of Ur', hp: 2400, maxHp: 2400 },
            { id: 2, x: 0.48, y: 0.32, type: 'granary', name: 'Imperial Silo', hp: 900, maxHp: 900 },
            { id: 3, x: 0.25, y: 0.48, type: 'souk', name: 'Artisan Souk', hp: 800, maxHp: 800 },
            { id: 4, x: 0.62, y: 0.58, type: 'smelter', name: 'Blast Foundry', hp: 1200, maxHp: 1200 },
            { id: 5, x: 0.22, y: 0.62, type: 'tenement', name: 'Mudbrick Slums', hp: 450, maxHp: 450 },
            { id: 6, x: 0.42, y: 0.52, type: 'barracks', name: 'Phalanx Garrison', hp: 1500, maxHp: 1500 }
        ];

        this.units = [
            { id: 101, x: 0.38, y: 0.42, targetX: 0.38, targetY: 0.42, type: 'villager', name: 'Royal Builder', hp: 50, maxHp: 50, team: 'blue', state: 'idle' },
            { id: 102, x: 0.44, y: 0.46, targetX: 0.44, targetY: 0.46, type: 'phalanx', name: 'Bronze Phalanx', hp: 160, maxHp: 160, team: 'blue', count: 600, state: 'idle' },
            { id: 103, x: 0.36, y: 0.54, targetX: 0.36, targetY: 0.54, type: 'archer', name: 'Composite Archers', hp: 90, maxHp: 90, team: 'blue', count: 350, state: 'idle' }
        ];

        this.enemyUnits = [];
        this.arrows = [];
        this.bloodFx = [];
        this.selectedEntity = this.buildings[0];

        this.initDOM();
        this.setupEvents();
        this.resize();
        this.updateResourceUI();
        this.updateConsoleUI();

        requestAnimationFrame((t) => this.loop(t));
    }

    initDOM() {
        this.epochCrest = document.getElementById('epoch-crest');
        this.epochName = document.getElementById('epoch-name');
        this.ageProgress = document.getElementById('age-progress-bar');
        this.resFood = document.getElementById('res-food');
        this.resWood = document.getElementById('res-wood');
        this.resGold = document.getElementById('res-gold');
        this.resStone = document.getElementById('res-stone');
        this.resPop = document.getElementById('res-pop');
        this.toast = document.getElementById('game-toast');
        this.toastTitle = document.getElementById('toast-title');
        this.toastDesc = document.getElementById('toast-desc');
        this.selectionPortrait = document.getElementById('selection-portrait');
        this.selectionTitle = document.getElementById('selection-title');
        this.selectionSubtitle = document.getElementById('selection-subtitle');
        this.statHp = document.getElementById('stat-hp');
        this.statGarrison = document.getElementById('stat-garrison');
        this.statImr = document.getElementById('stat-imr');
        this.statUnrest = document.getElementById('stat-unrest');
        this.commandGrid = document.getElementById('command-grid');
    }

    setupEvents() {
        window.addEventListener('resize', () => this.resize());

        document.getElementById('btn-menu-diplo')?.addEventListener('click', () => this.openModal('modal-diplomacy'));
        document.getElementById('btn-menu-demographics')?.addEventListener('click', () => this.openModal('modal-demographics'));
        document.getElementById('btn-menu-theme')?.addEventListener('click', () => this.openModal('modal-theme'));
        document.getElementById('btn-menu-savagery')?.addEventListener('click', () => this.openModal('modal-savagery'));
        document.getElementById('btn-menu-advance')?.addEventListener('click', () => this.advanceAge());

        document.querySelectorAll('.modal-close').forEach(btn => {
            btn.addEventListener('click', () => {
                const modalId = btn.dataset.close;
                document.getElementById(modalId)?.classList.add('hidden');
            });
        });

        document.querySelectorAll('.theme-card').forEach(card => {
            card.addEventListener('click', () => {
                document.querySelectorAll('.theme-card').forEach(c => c.classList.remove('active'));
                card.classList.add('active');
                this.currentTheme = card.dataset.theme;
                this.triggerToast("🎨 GRAPHICS KIT SWAPPED", "Swapped to " + THEMES[this.currentTheme].name);
            });
        });

        this.canvas.addEventListener('click', (e) => {
            const rect = this.canvas.getBoundingClientRect();
            const x = (e.clientX - rect.left) / rect.width;
            const y = (e.clientY - rect.top) / rect.height;

            if (this.activePlacingType) {
                this.buildings.push({
                    id: Date.now(),
                    x: x,
                    y: y,
                    type: this.activePlacingType,
                    name: this.activePlacingType.toUpperCase() + " #" + (this.buildings.length + 1),
                    hp: 800,
                    maxHp: 800
                });
                this.triggerToast("🏗️ CONSTRUCTION STARTED", "Placed " + this.activePlacingType.toUpperCase());
                this.activePlacingType = null;
                this.updateConsoleUI();
                return;
            }

            let clicked = this.units.find(u => Math.hypot(u.x - x, u.y - y) < 0.04);
            if (!clicked) {
                clicked = this.buildings.find(b => Math.hypot(b.x - x, b.y - y) < 0.05);
            }

            this.selectedEntity = clicked || null;
            this.updateConsoleUI();
        });

        this.canvas.addEventListener('contextmenu', (e) => {
            e.preventDefault();
            const rect = this.canvas.getBoundingClientRect();
            const x = (e.clientX - rect.left) / rect.width;
            const y = (e.clientY - rect.top) / rect.height;

            if (this.selectedEntity && this.selectedEntity.team === 'blue') {
                this.selectedEntity.targetX = x;
                this.selectedEntity.targetY = y;
                this.selectedEntity.state = 'moving';
                this.triggerToast("⚔️ ORDER DISPATCHED", "Moving to position (" + (x*100).toFixed(0) + ", " + (y*100).toFixed(0) + ")");
            }
        });

        window.addEventListener('keydown', (e) => {
            const key = e.key.toUpperCase();
            const tile = document.querySelector('.cmd-tile[data-hotkey="' + key + '"]');
            if (tile) tile.click();
        });

        document.getElementById('btn-tribute-ur')?.addEventListener('click', () => {
            if (this.resources.food >= 100) {
                this.resources.food -= 100;
                this.updateResourceUI();
                this.triggerToast("📜 TRIBUTE SENT", "Kingdom of Ur appreciated the 100 Grain Caravan (+15 Relations).");
            }
        });

        document.getElementById('btn-pact-ur')?.addEventListener('click', () => {
            this.triggerToast("🤝 ALLIANCE SEALED", "Mutual Defense Pact signed with King Mesannepada!");
        });

        document.getElementById('btn-declare-war-elam')?.addEventListener('click', () => {
            this.spawnEnemyHorde();
            document.getElementById('modal-diplomacy')?.classList.add('hidden');
            this.triggerToast("⚔️ WAR DECLARED", "Elamite Horde has crossed the river!");
        });

        document.getElementById('btn-fund-wells')?.addEventListener('click', () => {
            if (this.resources.gold >= 50) {
                this.resources.gold -= 50;
                this.updateResourceUI();
                this.statImr.textContent = "230 / 1,000 (23%)";
                this.triggerToast("⛲ SANITATION UPGRADE", "Deep well filtration built. Infant mortality reduced by 15%!");
            }
        });

        document.getElementById('btn-fund-midwives')?.addEventListener('click', () => {
            if (this.resources.gold >= 80) {
                this.resources.gold -= 80;
                this.updateResourceUI();
                this.triggerToast("🏥 MIDWIVES GUILD", "Midwifery training active. Child survival increased!");
            }
        });

        document.getElementById('btn-spoils-purge')?.addEventListener('click', () => {
            this.triggerToast("💀 SAVAGERY: TOTAL PURGE", "Settlement razed to the ground. World Coalition infamy spiked to 85%!");
            document.getElementById('modal-savagery')?.classList.add('hidden');
        });

        document.getElementById('btn-spoils-salt')?.addEventListener('click', () => {
            this.triggerToast("🧂 SAVAGERY: SALT THE EARTH", "Aquifers poisoned and fields salted.");
            document.getElementById('modal-savagery')?.classList.add('hidden');
        });
    }

    openModal(modalId) {
        document.querySelectorAll('.modal-backdrop').forEach(m => m.classList.add('hidden'));
        document.getElementById(modalId)?.classList.remove('hidden');
    }

    advanceAge() {
        this.currentEpoch = (this.currentEpoch + 1) % EPOCHS.length;
        const ep = EPOCHS[this.currentEpoch];
        this.epochCrest.textContent = ep.crest;
        this.epochName.textContent = ep.name;
        this.ageProgress.style.width = ep.progress + "%";
        this.triggerToast("⚡ EPOCH ADVANCEMENT", "Civilization entered the " + ep.name + "!");
    }

    spawnEnemyHorde() {
        this.enemyUnits = [
            { id: 201, x: 0.82, y: 0.28, targetX: 0.35, targetY: 0.38, type: 'horde', name: 'Elamite Raiders', hp: 150, maxHp: 150, team: 'red', count: 800, state: 'charging' },
            { id: 202, x: 0.85, y: 0.40, targetX: 0.44, targetY: 0.46, type: 'raider_archers', name: 'Nomad Archers', hp: 90, maxHp: 90, team: 'red', count: 400, state: 'charging' }
        ];
        this.triggerToast("🚨 ENEMY RAID DETECTED", "Hostile warband advancing on your temple!");
    }

    triggerToast(title, desc) {
        this.toastTitle.textContent = title;
        this.toastDesc.textContent = desc;
        this.toast.classList.remove('hidden');
        clearTimeout(this.toastTimeout);
        this.toastTimeout = setTimeout(() => this.toast.classList.add('hidden'), 4000);
    }

    updateResourceUI() {
        this.resFood.textContent = this.resources.food.toLocaleString();
        this.resWood.textContent = this.resources.wood.toLocaleString();
        this.resGold.textContent = this.resources.gold.toLocaleString();
        this.resStone.textContent = this.resources.stone.toLocaleString();
        this.resPop.textContent = this.resources.pop + " / " + this.resources.popCap;
    }

    updateConsoleUI() {
        const ent = this.selectedEntity;

        if (!ent) {
            this.selectionPortrait.textContent = "🌐";
            this.selectionTitle.textContent = "PROVINCIAL OVERVIEW";
            this.selectionSubtitle.textContent = "No Entity Selected (Click Map or Unit)";
            this.statHp.textContent = "---";
            this.statGarrison.textContent = this.units.length + " Regiments Active";
            this.statImr.textContent = "380 / 1,000 (38%)";
            this.statUnrest.textContent = "Low (20%)";

            this.renderCommandGrid([
                { hotkey: "Q", icon: "⚔️", label: "Select Army", cost: "", action: () => { this.selectedEntity = this.units[1]; this.updateConsoleUI(); } },
                { hotkey: "W", icon: "🚨", label: "Spawn Raiders", cost: "", action: () => this.spawnEnemyHorde() },
                { hotkey: "E", icon: "🌾", label: "Harvest Grain", cost: "+200🌾", action: () => { this.resources.food += 200; this.updateResourceUI(); this.triggerToast("🌾 HARVEST", "Gathered 200 grain."); } },
                { hotkey: "R", icon: "👑", label: "Diplomacy", cost: "", action: () => this.openModal('modal-diplomacy') },
                { hotkey: "T", icon: "🎨", label: "Themes", cost: "", action: () => this.openModal('modal-theme') },
                { hotkey: "A", icon: "💀", label: "Savagery", cost: "", action: () => this.openModal('modal-savagery') }
            ]);
            return;
        }

        this.selectionTitle.textContent = ent.name.toUpperCase();
        this.statHp.textContent = ent.hp + " / " + ent.maxHp;

        if (ent.type === 'ziggurat') {
            this.selectionPortrait.textContent = "🏛️";
            this.selectionSubtitle.textContent = "Town Center & Grand Temple";
            this.statGarrison.textContent = "12 Priests & Builders";
            this.statImr.textContent = "380 / 1,000 (38%)";
            this.statUnrest.textContent = "Low (20%)";

            this.renderCommandGrid([
                { hotkey: "Q", icon: "👷", label: "Train Builder", cost: "50🌾", action: () => this.trainUnit('villager') },
                { hotkey: "W", icon: "⛲", label: "Deep Wells", cost: "100🪵 50🪙", action: () => document.getElementById('btn-fund-wells')?.click() },
                { hotkey: "E", icon: "🏥", label: "Midwives", cost: "150🌾 80🪙", action: () => document.getElementById('btn-fund-midwives')?.click() },
                { hotkey: "R", icon: "⚡", label: "Advance Age", cost: "500🌾 200🪙", action: () => this.advanceAge() },
                { hotkey: "A", icon: "🔔", label: "Town Bell", cost: "", action: () => this.triggerToast("🔔 TOWN BELL", "Citizens garrisoned!") }
            ]);
        } else if (ent.type === 'villager') {
            this.selectionPortrait.textContent = "👷";
            this.selectionSubtitle.textContent = "Builder & Laborer";
            this.statGarrison.textContent = "Carrying 15 Timber";
            this.statImr.textContent = "---";
            this.statUnrest.textContent = "Loyal";

            this.renderCommandGrid([
                { hotkey: "Q", icon: "🍞", label: "Build Granary", cost: "100🪵", action: () => this.startPlacing('granary') },
                { hotkey: "W", icon: "🏚️", label: "Build Tenement", cost: "50🪵 30🌾", action: () => this.startPlacing('tenement') },
                { hotkey: "E", icon: "🏪", label: "Build Souk", cost: "120🪵 60🪙", action: () => this.startPlacing('souk') },
                { hotkey: "R", icon: "⚔️", label: "Build Barracks", cost: "150🪵 50🧱", action: () => this.startPlacing('barracks') },
                { hotkey: "A", icon: "🌋", label: "Build Smelter", cost: "200🧱 100🪙", action: () => this.startPlacing('smelter') }
            ]);
        } else if (ent.type === 'barracks') {
            this.selectionPortrait.textContent = "⚔️";
            this.selectionSubtitle.textContent = "Military Barracks & Armory";
            this.statGarrison.textContent = "Arming Bronze Legions";
            this.statImr.textContent = "---";
            this.statUnrest.textContent = "Strict Discipline";

            this.renderCommandGrid([
                { hotkey: "Q", icon: "🛡️", label: "Train Phalanx", cost: "60🌾 20🧱", action: () => this.trainUnit('phalanx') },
                { hotkey: "W", icon: "🏹", label: "Train Archer", cost: "50🌾 35🪵", action: () => this.trainUnit('archer') },
                { hotkey: "E", icon: "🌾", label: "Draft Levy", cost: "20🌾", action: () => { this.resources.food -= 20; this.triggerToast("🎺 LEVY DRAFTED", "+500 Conscripts mobilized."); } },
                { hotkey: "R", icon: "🗡️", label: "Bronze Weapons", cost: "150🪙", action: () => this.triggerToast("🗡️ RESEARCH COMPLETE", "Infantry attack +2.") }
            ]);
        } else if (ent.team === 'blue') {
            this.selectionPortrait.textContent = ent.type === 'phalanx' ? "🛡️" : "🏹";
            this.selectionSubtitle.textContent = "Regiment Strength: " + ent.count + " Men";
            this.statGarrison.textContent = "Baggage Supply: 100%";
            this.statImr.textContent = "---";
            this.statUnrest.textContent = "Morale: High";

            this.renderCommandGrid([
                { hotkey: "Q", icon: "⚔️", label: "Charge", cost: "", action: () => this.orderCharge() },
                { hotkey: "W", icon: "📦", label: "Box Formation", cost: "", action: () => this.triggerToast("🛡️ FORMATION", "Locked into Defensive Box.") },
                { hotkey: "E", icon: "🏹", label: "Volley Fire", cost: "", action: () => this.orderVolley() },
                { hotkey: "R", icon: "🔥", label: "Pillage", cost: "", action: () => this.triggerToast("🔥 SACKING", "Pillaging enemy farmsteads!") }
            ]);
        } else {
            this.selectionPortrait.textContent = "🏠";
            this.selectionSubtitle.textContent = "Civilian Infrastructure";
            this.statGarrison.textContent = "Active Worksite";
            this.statImr.textContent = "---";
            this.statUnrest.textContent = "Productive";
            this.renderCommandGrid([]);
        }
    }

    renderCommandGrid(actions) {
        this.commandGrid.innerHTML = '';
        for (let i = 0; i < 15; i++) {
            const action = actions[i];
            const tile = document.createElement('div');
            tile.className = 'cmd-tile' + (action ? '' : ' empty');

            if (action) {
                tile.dataset.hotkey = action.hotkey;
                tile.innerHTML = '<span class="cmd-hotkey">' + action.hotkey + '</span><span class="cmd-icon">' + action.icon + '</span><span class="cmd-cost">' + action.cost + '</span>';
                tile.title = action.label + " [" + action.hotkey + "] " + action.cost;
                tile.addEventListener('click', action.action);
            }
            this.commandGrid.appendChild(tile);
        }
    }

    startPlacing(type) {
        this.activePlacingType = type;
        this.triggerToast("🏗️ SELECT LOCATION", "Click on the map to place " + type.toUpperCase());
    }

    trainUnit(type) {
        if (type === 'villager' && this.resources.food >= 50) {
            this.resources.food -= 50;
            this.resources.pop++;
            this.units.push({
                id: Date.now(),
                x: 0.38 + (Math.random() - 0.5) * 0.05,
                y: 0.42 + (Math.random() - 0.5) * 0.05,
                targetX: 0.38,
                targetY: 0.42,
                type: 'villager',
                name: 'Royal Builder',
                hp: 50,
                maxHp: 50,
                team: 'blue',
                state: 'idle'
            });
            this.updateResourceUI();
            this.triggerToast("👷 TRAINED BUILDER", "Builder ready for orders.");
        } else if (type === 'phalanx' && this.resources.food >= 60) {
            this.resources.food -= 60;
            this.resources.pop += 2;
            this.units.push({
                id: Date.now(),
                x: 0.44 + (Math.random() - 0.5) * 0.05,
                y: 0.52 + (Math.random() - 0.5) * 0.05,
                targetX: 0.44,
                targetY: 0.52,
                type: 'phalanx',
                name: 'Bronze Phalanx',
                hp: 160,
                maxHp: 160,
                team: 'blue',
                count: 500,
                state: 'idle'
            });
            this.updateResourceUI();
            this.triggerToast("🛡️ PHALANX MUSTERED", "Bronze Phalanx deployed to the field.");
        } else if (type === 'archer' && this.resources.food >= 50) {
            this.resources.food -= 50;
            this.resources.pop += 2;
            this.units.push({
                id: Date.now(),
                x: 0.36 + (Math.random() - 0.5) * 0.05,
                y: 0.56 + (Math.random() - 0.5) * 0.05,
                targetX: 0.36,
                targetY: 0.56,
                type: 'archer',
                name: 'Composite Archers',
                hp: 90,
                maxHp: 90,
                team: 'blue',
                count: 350,
                state: 'idle'
            });
            this.updateResourceUI();
            this.triggerToast("🏹 ARCHERS MUSTERED", "Archer company ready.");
        }
    }

    orderCharge() {
        if (this.enemyUnits.length === 0) return;
        const enemy = this.enemyUnits[0];
        this.units.forEach(u => {
            if (u.team === 'blue') {
                u.targetX = enemy.x;
                u.targetY = enemy.y;
                u.state = 'charging';
            }
        });
        this.triggerToast("⚔️ CHARGE ORDER", "All regiments closing with enemy!");
    }

    orderVolley() {
        this.units.forEach(u => {
            if (u.type === 'archer') {
                this.enemyUnits.forEach(enemy => {
                    for (let i = 0; i < 12; i++) {
                        this.arrows.push({
                            x: u.x,
                            y: u.y,
                            startX: u.x,
                            startY: u.y,
                            targetX: enemy.x + (Math.random() - 0.5) * 0.04,
                            targetY: enemy.y + (Math.random() - 0.5) * 0.04,
                            progress: 0,
                            speed: 0.04
                        });
                    }
                });
            }
        });
    }

    resize() {
        const rect = this.canvas.parentElement.getBoundingClientRect();
        this.canvas.width = rect.width;
        this.canvas.height = rect.height;
    }

    loop(timestamp) {
        this.time += 0.016;
        this.updateSimulation();
        this.renderViewport();
        this.renderMinimap();
        requestAnimationFrame((t) => this.loop(t));
    }

    updateSimulation() {
        const speed = 0.0015;

        this.units.forEach(u => {
            const dx = u.targetX - u.x;
            const dy = u.targetY - u.y;
            const dist = Math.hypot(dx, dy);
            if (dist > 0.005) {
                u.x += (dx / dist) * speed;
                u.y += (dy / dist) * speed;
            }
        });

        this.enemyUnits.forEach(r => {
            const dx = r.targetX - r.x;
            const dy = r.targetY - r.y;
            const dist = Math.hypot(dx, dy);
            if (dist > 0.005) {
                r.x += (dx / dist) * (speed * 1.1);
                r.y += (dy / dist) * (speed * 1.1);
            }

            this.units.forEach(b => {
                if (b.team === 'blue' && Math.hypot(r.x - b.x, r.y - b.y) < 0.04) {
                    r.hp -= 0.3;
                    b.hp -= 0.2;
                    if (Math.random() < 0.25) {
                        this.bloodFx.push({
                            x: (r.x + b.x)/2 + (Math.random() - 0.5) * 0.02,
                            y: (r.y + b.y)/2 + (Math.random() - 0.5) * 0.02,
                            life: 1.0
                        });
                    }
                }
            });
        });

        this.enemyUnits = this.enemyUnits.filter(e => e.hp > 0);
        this.units = this.units.filter(u => u.hp > 0);

        for (let i = this.arrows.length - 1; i >= 0; i--) {
            const a = this.arrows[i];
            a.progress += a.speed;
            a.x = a.startX + (a.targetX - a.startX) * a.progress;
            a.y = a.startY + (a.targetY - a.startY) * a.progress;
            if (a.progress >= 1.0) {
                this.bloodFx.push({ x: a.targetX, y: a.targetY, life: 1.0 });
                this.arrows.splice(i, 1);
            }
        }

        for (let i = this.bloodFx.length - 1; i >= 0; i--) {
            this.bloodFx[i].life -= 0.01;
            if (this.bloodFx[i].life <= 0) this.bloodFx.splice(i, 1);
        }
    }

    renderViewport() {
        const w = this.canvas.width;
        const h = this.canvas.height;
        const theme = THEMES[this.currentTheme];

        this.ctx.fillStyle = theme.bg;
        this.ctx.fillRect(0, 0, w, h);

        this.ctx.fillStyle = theme.sand;
        this.ctx.beginPath();
        this.ctx.ellipse(w * 0.42, h * 0.48, w * 0.45, h * 0.42, 0, 0, Math.PI * 2);
        this.ctx.fill();

        this.ctx.fillStyle = theme.banks;
        this.ctx.beginPath();
        this.ctx.moveTo(w * 0.52, 0);
        this.ctx.bezierCurveTo(w * 0.58, h * 0.35, w * 0.68, h * 0.65, w * 0.62, h);
        this.ctx.lineTo(w * 0.78, h);
        this.ctx.bezierCurveTo(w * 0.82, h * 0.65, w * 0.72, h * 0.35, w * 0.68, 0);
        this.ctx.closePath();
        this.ctx.fill();

        this.ctx.fillStyle = theme.river;
        this.ctx.beginPath();
        this.ctx.moveTo(w * 0.56, 0);
        this.ctx.bezierCurveTo(w * 0.62, h * 0.35, w * 0.72, h * 0.65, w * 0.66, h);
        this.ctx.lineTo(w * 0.72, h);
        this.ctx.bezierCurveTo(w * 0.78, h * 0.65, w * 0.68, h * 0.35, w * 0.62, 0);
        this.ctx.closePath();
        this.ctx.fill();

        this.buildings.forEach(b => {
            this.renderBuilding(b, theme);
        });

        this.bloodFx.forEach(p => {
            this.ctx.fillStyle = "rgba(220, 38, 38, " + p.life + ")";
            this.ctx.beginPath();
            this.ctx.arc(p.x * w, p.y * h, 4 * p.life, 0, Math.PI * 2);
            this.ctx.fill();
        });

        this.ctx.strokeStyle = "#fef08a";
        this.ctx.lineWidth = 2;
        this.arrows.forEach(a => {
            this.ctx.beginPath();
            this.ctx.moveTo(a.x * w, a.y * h);
            this.ctx.lineTo(a.x * w - 6, a.y * h - 4);
            this.ctx.stroke();
        });

        this.units.forEach(u => {
            const ux = u.x * w;
            const uy = u.y * h;
            const isSel = this.selectedEntity === u;

            this.ctx.fillStyle = "#2563eb";
            this.ctx.strokeStyle = isSel ? "#fbbf24" : "#93c5fd";
            this.ctx.lineWidth = isSel ? 2.5 : 1.5;

            this.ctx.fillRect(ux - 12, uy - 10, 24, 20);
            this.ctx.strokeRect(ux - 12, uy - 10, 24, 20);

            this.ctx.strokeStyle = "#dbeafe";
            for (let sx = -8; sx <= 8; sx += 5) {
                this.ctx.beginPath();
                this.ctx.moveTo(ux + sx, uy - 10);
                this.ctx.lineTo(ux + sx + 4, uy - 20);
                this.ctx.stroke();
            }

            this.ctx.fillStyle = "#1e293b";
            this.ctx.fillRect(ux - 12, uy - 16, 24, 3);
            this.ctx.fillStyle = "#10b981";
            this.ctx.fillRect(ux - 12, uy - 16, 24 * (u.hp / u.maxHp), 3);
        });

        this.enemyUnits.forEach(r => {
            const rx = r.x * w;
            const ry = r.y * h;

            this.ctx.fillStyle = "#dc2626";
            this.ctx.strokeStyle = "#fca5a5";
            this.ctx.lineWidth = 1.5;

            this.ctx.fillRect(rx - 12, ry - 10, 24, 20);
            this.ctx.strokeRect(rx - 12, ry - 10, 24, 20);

            this.ctx.strokeStyle = "#f87171";
            for (let sx = -8; sx <= 8; sx += 5) {
                this.ctx.beginPath();
                this.ctx.moveTo(rx + sx, ry - 10);
                this.ctx.lineTo(rx + sx - 3, ry - 18);
                this.ctx.stroke();
            }

            this.ctx.fillStyle = "#1e293b";
            this.ctx.fillRect(rx - 12, ry - 16, 24, 3);
            this.ctx.fillStyle = "#ef4444";
            this.ctx.fillRect(rx - 12, ry - 16, 24 * (r.hp / r.maxHp), 3);
        });
    }

    renderBuilding(b, theme) {
        const w = this.canvas.width;
        const h = this.canvas.height;
        const bx = b.x * w;
        const by = b.y * h;
        const isSel = this.selectedEntity === b;

        this.ctx.save();

        if (isSel) {
            this.ctx.strokeStyle = "#fbbf24";
            this.ctx.lineWidth = 2;
            this.ctx.beginPath();
            this.ctx.ellipse(bx, by + 10, 36, 18, 0, 0, Math.PI * 2);
            this.ctx.stroke();
        }

        if (b.type === 'ziggurat') {
            this.ctx.fillStyle = theme.wall;
            this.ctx.fillRect(bx - 32, by - 18, 64, 36);
            this.ctx.strokeStyle = theme.trim;
            this.ctx.lineWidth = 1.5;
            this.ctx.strokeRect(bx - 32, by - 18, 64, 36);

            this.ctx.fillStyle = theme.roof;
            this.ctx.fillRect(bx - 22, by - 32, 44, 18);
            this.ctx.strokeRect(bx - 22, by - 32, 44, 18);

            this.ctx.fillStyle = theme.trim;
            this.ctx.fillRect(bx - 10, by - 44, 20, 14);

            this.ctx.fillStyle = "#ef4444";
            this.ctx.beginPath();
            this.ctx.arc(bx, by - 48, 4 + Math.sin(this.time * 6) * 1.5, 0, Math.PI * 2);
            this.ctx.fill();

        } else if (b.type === 'granary') {
            this.ctx.fillStyle = theme.wall;
            this.ctx.beginPath();
            this.ctx.ellipse(bx, by, 16, 20, 0, 0, Math.PI * 2);
            this.ctx.fill();
            this.ctx.strokeStyle = theme.trim;
            this.ctx.stroke();

            this.ctx.fillStyle = theme.roof;
            this.ctx.beginPath();
            this.ctx.moveTo(bx - 16, by - 10);
            this.ctx.lineTo(bx + 16, by - 10);
            this.ctx.lineTo(bx, by - 28);
            this.ctx.closePath();
            this.ctx.fill();

        } else if (b.type === 'smelter') {
            this.ctx.fillStyle = "#27272a";
            this.ctx.fillRect(bx - 16, by - 14, 32, 28);
            this.ctx.strokeStyle = "#f97316";
            this.ctx.strokeRect(bx - 16, by - 14, 32, 28);

            this.ctx.fillStyle = "#ea580c";
            this.ctx.fillRect(bx - 6, by - 2, 12, 12);

            this.ctx.fillStyle = theme.vfx;
            for (let i = 0; i < 3; i++) {
                const sy = by - 18 - ((this.time * 20 + i * 15) % 35);
                const sx = bx + Math.sin(sy * 0.1) * 6;
                this.ctx.beginPath();
                this.ctx.arc(sx, sy, 3 + i * 2, 0, Math.PI * 2);
                this.ctx.fill();
            }

        } else if (b.type === 'barracks') {
            this.ctx.fillStyle = theme.wall;
            this.ctx.fillRect(bx - 22, by - 12, 44, 26);
            this.ctx.strokeStyle = theme.trim;
            this.ctx.strokeRect(bx - 22, by - 12, 44, 26);

            this.ctx.fillStyle = theme.roof;
            for (let step = -18; step <= 18; step += 9) {
                this.ctx.fillRect(bx + step, by - 18, 6, 6);
            }
        } else {
            this.ctx.fillStyle = theme.wall;
            this.ctx.fillRect(bx - 12, by - 10, 24, 20);
            this.ctx.fillStyle = theme.roof;
            this.ctx.fillRect(bx - 14, by - 15, 28, 6);
        }

        this.ctx.fillStyle = "#cbd5e1";
        this.ctx.font = "8px Rajdhani, sans-serif";
        this.ctx.fillText(b.type.toUpperCase(), bx - 12, by + 24);

        this.ctx.restore();
    }

    renderMinimap() {
        const mw = this.minimapCanvas.width;
        const mh = this.minimapCanvas.height;

        this.miniCtx.fillStyle = "#0c1017";
        this.miniCtx.fillRect(0, 0, mw, mh);

        this.miniCtx.fillStyle = "#0284c7";
        this.miniCtx.beginPath();
        this.miniCtx.moveTo(mw * 0.58, 0);
        this.miniCtx.bezierCurveTo(mw * 0.65, mh * 0.4, mw * 0.72, mh * 0.7, mw * 0.68, mh);
        this.miniCtx.lineTo(mw * 0.74, mh);
        this.miniCtx.bezierCurveTo(mw * 0.78, mh * 0.7, mw * 0.70, mh * 0.4, mw * 0.64, 0);
        this.miniCtx.fill();

        this.miniCtx.fillStyle = "#38bdf8";
        this.units.forEach(u => {
            this.miniCtx.fillRect(u.x * mw - 2, u.y * mh - 2, 4, 4);
        });

        this.miniCtx.fillStyle = "#fbbf24";
        this.buildings.forEach(b => {
            this.miniCtx.fillRect(b.x * mw - 2, b.y * mh - 2, 5, 5);
        });

        this.miniCtx.fillStyle = "#ef4444";
        this.enemyUnits.forEach(r => {
            this.miniCtx.fillRect(r.x * mw - 2, r.y * mh - 2, 4, 4);
        });

        this.miniCtx.strokeStyle = "rgba(255, 255, 255, 0.4)";
        this.miniCtx.strokeRect(mw * 0.15, mh * 0.15, mw * 0.7, mh * 0.7);
    }
}

window.addEventListener('DOMContentLoaded', () => {
    new DominionRTSApp();
});
