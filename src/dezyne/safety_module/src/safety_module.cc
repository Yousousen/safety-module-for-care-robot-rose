#include "safety_module.hh"

// Global variables and helper functions.
namespace {
    Behavior_t b = SAFE;
    struct fb_t *fb;
    ErrorCode_t ret = OK;
    int fbfd = 0;

    // Check if device is framebuffer.
    static int is_framebuffer_device(const struct dirent *dir)
    {
        return strncmp(FB_DEV_NAME, dir->d_name,
                strlen(FB_DEV_NAME)-1) == 0;
    }

    // Open framebuffer device.
    static int open_fbdev(const char *dev_name) {
        struct dirent **namelist;
        int i, ndev;
        int fd = -1;
        struct fb_fix_screeninfo fix_info;

        ndev = scandir(DEV_FB, &namelist, is_framebuffer_device, versionsort);
        if (ndev <= 0)
            return ndev;

        for (i = 0; i < ndev; i++) {
            char fname[64];

            snprintf(fname, sizeof(fname),
                    "%s/%s", DEV_FB, namelist[i]->d_name);
            fd = open(fname, O_RDWR);
            if (fd < 0)
                continue;
            ioctl(fd, FBIOGET_FSCREENINFO, &fix_info);
            if (strcmp(dev_name, fix_info.id) == 0)
                break;
            close(fd);
            fd = -1;
        }
        for (i = 0; i < ndev; i++)
            free(namelist[i]);

        return fd;
    }
}

SafetyModule::SafetyModule() {
    // Create a rose representation
    this->rose = new CareRobotRose();

    if (CSV_HAS_POSITION) {
        // Load from CSV.
    } else {
        position = new Position(11.0, 12.0, 13.0, 14.0);
    }

    if (CSV_HAS_VELOCITY) {
        // Load from CSV.
    } else {
        velocity = new Velocity(11.0, 12.0, 13.0, 14.0);
    }

    if (CSV_HAS_ACCELERATION) {
        // Load from CSV.
    } else {
        acceleration = new Acceleration(11.0, 12.0, 13.0, 14.0);
    }

    if (CSV_HAS_ANGULAR_ACCELERATION) {
        // Load from CSV.
    } else {
        angular_acceleration = new AngularAcceleration(11.0, 12.0, 13.0, 14.0);
    }

    if (CSV_HAS_ANGULAR_VELOCITY) {
        // Load from CSV.
    } else {
        angular_velocity = new AngularVelocity(11.0, 12.0, 13.0, 14.0);
    }

    if (CSV_HAS_ANGULAR_ACCELERATION) {
        // Load from CSV.
    } else {
        angular_acceleration = new AngularAcceleration(11.0, 12.0, 13.0, 14.0);
    }

}

SafetyModule::~SafetyModule() {
    if (rose != nullptr)                 delete rose;
    if (position != nullptr)             delete position;
    if (velocity != nullptr)             delete velocity;
    if (acceleration != nullptr)         delete acceleration;
    if (angular_velocity != nullptr)     delete angular_velocity;
    if (angular_acceleration != nullptr) delete angular_acceleration;
}


void SafetyModule::light_led(struct fb_t* fb, unsigned color) {
    for (int i = 0; i < 8; ++i) {
        for (int j = 0; j < 8; ++j) {
            fb->pixel[i][j]= color;
        }
    }
}

void SafetyModule::initialise_framebuffer() {

    // Initialise frame buffer for led matrix.
    std::cout << "Initialising framebuffer...\n";
    fbfd = open_fbdev("RPi-Sense FB");
    if (fbfd <= 0) {
        ret = NOT_OK;
        std::cerr << "Error: cannot open framebuffer device.\n";
        return;
    }
    fb = (struct fb_t*) mmap(0, 128, PROT_READ | PROT_WRITE, MAP_SHARED, fbfd, 0);
    if (!fb) {
        ret = NOT_OK;
        std::cerr << "Failed to mmap.\n";
        return;
    }
    memset(fb, 0, 128);
    std::cout << "Framebuffer initialised.\n";
}

void SafetyModule::destruct_framebuffer() {
    memset(fb, 0, 128);
    munmap(fb, 128);
    close(fbfd);
}

ErrorCode_t SafetyModule::roll() {
    // Initialise dezyne locator and runtime.
    dzn::locator loc;
    dzn::runtime rt;
    loc.set(rt);

    System s(loc);
    s.dzn_meta.name = "System";

    // Bind native functions
    s.iController.out.initialise_framebuffer = initialise_framebuffer;
    s.iController.out.destruct_framebuffer = destruct_framebuffer;
    s.iController.out.light_led = light_led;

    // Check bindings
    s.check_bindings();


    // Initialise framebuffer
    s.iController.in.initialise();

    // Run indefinitely unless input is equal to "q".
    std::cout << "Started running indefinitely. press q<enter> to quit.\n";
    std::string input;
    while (1) {
        std::cin >> input;
        if (input == "q") {
            break;
        } else if (input == "r") {
            s.iController.in.light_red(fb);
        } else if (input == "b") {
            s.iController.in.light_blue(fb);
        } else if (input == "a") {
            switch (this->check_acceleration()) {
                case SAFE:
                    s.iController.in.safe_acceleration(fb);
                    break;
                case UNSAFE:
                    s.iController.in.unsafe_acceleration(fb);
            }
        } else if (input == "i") {
            // Purposely here to show illegal exception handler.
            s.iController.in.initialise();
        }
    }
    // Destructs framebuffer
    s.iController.in.destruct();
}


void SafetyModule::retrieve_position() {
    auto previous = this->position->current;
    srand((unsigned)time(NULL));
    this->position->current = rand() % 100 + 1;
    this->position->change = this->position->current - previous;
}

void SafetyModule::retrieve_velocity() {
    auto previous = this->velocity->current;
    srand((unsigned)time(NULL));
    this->velocity->current = rand() % 100 + 1;
    this->velocity->change = this->velocity->current - previous;
}

void SafetyModule::retrieve_acceleration() {
    auto previous = this->acceleration->current;
    srand((unsigned)time(NULL));
    this->acceleration->current = rand() % 100 + 1;
    this->acceleration->change = this->acceleration->current - previous;
}

void SafetyModule::retrieve_angular_velocity() {
    auto previous = this->angular_velocity->current;
    srand((unsigned)time(NULL));
    this->angular_velocity->current = rand() % 100 + 1;
    this->angular_velocity->change = this->angular_velocity->current - previous;
}


void SafetyModule::retrieve_angular_acceleration() {
    auto previous = this->angular_acceleration->current;
    srand((unsigned)time(NULL));
    this->angular_acceleration->current = rand() % 100 + 1;
    this->angular_acceleration->change = this->angular_acceleration->current -
        previous;
}

void SafetyModule::retrieve_all() {
    this->retrieve_position();
    this->retrieve_velocity();
    this->retrieve_acceleration();
    this->retrieve_angular_velocity();
    this->retrieve_angular_acceleration();
}

Behavior_t SafetyModule::check_acceleration() {
    // Update acceleration
    this->retrieve_acceleration();
    if (this->acceleration->current > 50)
        return UNSAFE;
    return SAFE;
}

std::ostream& operator<< (std::ostream& os, const SafetyModule&
        safety_module) {
    os  << "##### Safety Module #####:\n\n"
        << *safety_module.position
        << *safety_module.velocity
        << *safety_module.acceleration
        << *safety_module.angular_velocity
        << *safety_module.angular_acceleration
        ;

    return os;
}
