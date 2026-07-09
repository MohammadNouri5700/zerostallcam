plugins {
    alias(libs.plugins.android.application)
}

android {
    namespace = "com.example.zerostallcam"
    compileSdk {
        version = release(37)
    }

    defaultConfig {
        applicationId = "com.example.zerostallcam"
        minSdk = 26
        targetSdk = 37
        versionCode = 1
        versionName = "1.0"

        testInstrumentationRunner = "androidx.test.runner.AndroidJUnitRunner"
    }

    buildTypes {
        debug {
            externalNativeBuild {
                cmake {
                    cppFlags("-DMEASUREMENT_ENABLED=true")
                }
            }
        }
        create("measurement") {
            initWith(getByName("debug"))
            externalNativeBuild {
                cmake {
                    cppFlags("-DMEASUREMENT_ENABLED=true")
                }
            }
            buildConfigField("boolean", "MEASUREMENT", "true")
        }
        release {
            optimization {
                enable = false
            }
            buildConfigField("boolean", "MEASUREMENT", "true")
            signingConfig = signingConfigs.getByName("debug")
        }
    }
    compileOptions {
        sourceCompatibility = JavaVersion.VERSION_11
        targetCompatibility = JavaVersion.VERSION_11
    }
    externalNativeBuild {
        cmake {
            path = file("src/main/cpp/CMakeLists.txt")
            version = "3.22.1"
        }
    }
    buildFeatures {
        viewBinding = true
        buildConfig = true
    }
}

dependencies {
    implementation(libs.androidx.appcompat)
    implementation(libs.androidx.constraintlayout)
    implementation(libs.androidx.core.ktx)
    implementation(libs.material)
    testImplementation(libs.junit)
    androidTestImplementation(libs.androidx.espresso.core)
    androidTestImplementation(libs.androidx.junit)
//    debugImplementation(libs.leakcanary)
}