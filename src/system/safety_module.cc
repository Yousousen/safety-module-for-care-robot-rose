#include "safety_module.hh"

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

ErrorCode_t SafetyModule::light_led(struct fb_t* fb, LedColor_t color) {
    for (int i = 0; i < 8; ++i) {
        for (int j = 0; j < 8; ++j) {
            fb->pixel[i][j]= color;
        }
    }
    return OK;
}


ErrorCode_t SafetyModule::roll() {
    std::string input;
    Behavior_t b = SAFE;
    struct fb_t *fb;
	ErrorCode_t ret = OK;
	int fbfd = 0;

    // Initialise frame buffer for led matrix.
    std::cout << "Initialising framebuffer...\n";
	fbfd = open_fbdev("RPi-Sense FB");
	if (fbfd <= 0) {
		ret = NOT_OK;
        std::cerr << "Error: cannot open framebuffer device.\n";
		goto err_fb; 
		
	}
	fb = (struct fb_t*) mmap(0, 128, PROT_READ | PROT_WRITE, MAP_SHARED, fbfd, 0);
	if (!fb) {
		ret = NOT_OK;
        std::cerr << "Failed to mmap.\n";
		goto err_fb;
	}
	memset(fb, 0, 128);
    light_led(fb, GREEN);
    std::cout << "Framebuffer initialised.\n";

    // Run indefinitely unless input is equal to "q".
    std::cout << "Started running indefinitely. press q<enter> to quit.\n";
    while (1) {
        std::cin >> input;
        if (input == "q") {
            break;
        } else if (input == "a") {
            b = this->retrieve_acceleration();
        } else if (input == "s") {
            b = this->rose->grip_arm->retrieve_strength();
        } else if (input == "force") {
            b = UNSAFE;
        } else if (input == "fix") {
            b = SAFE;
        }

        if (b == UNSAFE) {
            light_led(fb, RED);
            b = SAFE;
        } else {
            light_led(fb, GREEN);
        }

    }

	memset(fb, 0, 128);
	munmap(fb, 128);
err_fb:
    close(fbfd);
    return ret;
}


Behavior_t SafetyModule::retrieve_position() {
    Behavior_t isSafeBehavior = SAFE;
    auto previous = this->position->current;
    srand((unsigned)time(NULL));

    if ((this->position->current = rand() % 100 + 1) > 50) {
        isSafeBehavior = UNSAFE;
    } else {
        isSafeBehavior = SAFE;
    }

    this->position->change = this->position->current - previous;
    return isSafeBehavior;
}

Behavior_t SafetyModule::retrieve_velocity() {
    Behavior_t isSafeBehavior = SAFE;
    auto previous = this->velocity->current;
    srand((unsigned)time(NULL));

    if ((this->velocity->current = rand() % 100 + 1) > 50) {
        isSafeBehavior = UNSAFE;
    } else {
        isSafeBehavior = SAFE;
    }

    this->velocity->change = this->velocity->current - previous;
    return isSafeBehavior;
}

Behavior_t SafetyModule::retrieve_acceleration() {
    Behavior_t isSafeBehavior = SAFE;
    auto previous = this->acceleration->current;
    srand((unsigned)time(NULL));

    if ((this->acceleration->current = rand() % 100 + 1) > 50) {
        isSafeBehavior = UNSAFE;
    } else {
        isSafeBehavior = SAFE;
    }

    this->acceleration->change = this->acceleration->current - previous;
    return isSafeBehavior;
}

Behavior_t SafetyModule::retrieve_angular_velocity() {
    Behavior_t isSafeBehavior = SAFE;
    auto previous = this->angular_velocity->current;
    srand((unsigned)time(NULL));
    if ((this->angular_velocity->current = rand() % 100 + 1) > 50) {
        isSafeBehavior = UNSAFE;
    } else {
        isSafeBehavior = SAFE;
    }
    this->angular_velocity->change = this->angular_velocity->current - previous;
    return isSafeBehavior;
}


Behavior_t SafetyModule::retrieve_angular_acceleration() {
    Behavior_t isSafeBehavior = SAFE;
    auto previous = this->angular_acceleration->current;
    srand((unsigned)time(NULL));

    if ((this->angular_acceleration->current = rand() % 100 + 1) > 50) {
        isSafeBehavior = UNSAFE;
    } else {
        isSafeBehavior = SAFE;
    }

    this->angular_acceleration->change = this->angular_acceleration->current -
        previous;
    return isSafeBehavior;
}

Behavior_t SafetyModule::retrieve_all() {
    if (this->retrieve_position() == UNSAFE) return UNSAFE;
    if (this->retrieve_velocity() == UNSAFE) return UNSAFE;
    if (this->retrieve_acceleration() == UNSAFE) return UNSAFE;
    if (this->retrieve_angular_velocity() == UNSAFE) return UNSAFE;
    if (this->retrieve_angular_acceleration() == UNSAFE) return UNSAFE;
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

